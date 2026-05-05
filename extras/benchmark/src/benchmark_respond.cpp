////////////////////////////////////////////////////////////////////////////////
// File: benchmark_respond.cpp                                                //
// Project: respond                                                           //
// Created Date: 2026-04-27                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-04-27                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include <Eigen/Dense>

#include <respond/model.hpp>
#include <respond/transition_factory.hpp>

namespace {
using Clock = std::chrono::steady_clock;
using Nanoseconds = std::chrono::duration<double, std::nano>;

#if defined(__clang__) || defined(__GNUC__)
template <typename T> inline void DoNotOptimize(const T &value) {
    asm volatile("" : : "g"(&value) : "memory");
}

inline void ClobberMemory() { asm volatile("" : : : "memory"); }
#else
template <typename T> inline void DoNotOptimize(const T &value) {
    volatile const T *sink = &value;
    (void)sink;
}

inline void ClobberMemory() {
    std::atomic_signal_fence(std::memory_order_seq_cst);
}
#endif

struct BenchmarkConfig {
    std::size_t state_size = 200;
    int steps = 365;
    int warmup_iterations = 5;
    int sample_iterations = 25;
    int repetitions = 3;
    int history_capture_interval = 1;
};

struct TimedRunResult {
    double elapsed_ns = 0.0;
    double checksum = 0.0;
    std::size_t recorded_points = 0;
};

struct Statistics {
    double min_ns = 0.0;
    double max_ns = 0.0;
    double mean_ns = 0.0;
    double median_ns = 0.0;
    double p95_ns = 0.0;
    double stddev_ns = 0.0;
};

double ToMilliseconds(double ns) { return ns / 1'000'000.0; }

std::string GetBuildType() {
#ifdef NDEBUG
    return "Release";
#else
    return "Debug";
#endif
}

std::string GetCompilerString() {
#if defined(__clang__)
    return std::string("Clang ") + __clang_version__;
#elif defined(__GNUC__)
    return std::string("GCC ") + __VERSION__;
#elif defined(_MSC_VER)
    return std::string("MSVC ") + std::to_string(_MSC_VER);
#else
    return "Unknown compiler";
#endif
}

std::string GetCpuModel() {
#if defined(__linux__)
    std::ifstream cpuinfo("/proc/cpuinfo");
    if (!cpuinfo.is_open()) {
        return "unknown";
    }

    std::string line;
    while (std::getline(cpuinfo, line)) {
        const std::string key = "model name";
        const auto pos = line.find(key);
        if (pos == std::string::npos) {
            continue;
        }
        const auto colon_pos = line.find(':', pos + key.size());
        if (colon_pos == std::string::npos) {
            continue;
        }
        std::string value = line.substr(colon_pos + 1);
        const auto first = value.find_first_not_of(" \t");
        if (first == std::string::npos) {
            continue;
        }
        value = value.substr(first);
        const auto last = value.find_last_not_of(" \t");
        if (last != std::string::npos) {
            value = value.substr(0, last + 1);
        }
        if (!value.empty()) {
            return value;
        }
    }
#endif
    return "unknown";
}

void PrintEnvironmentMetadata() {
    std::ostringstream pointer_bits;
    pointer_bits << (sizeof(void *) * 8U) << "-bit";

    std::cout << "Environment\n"
              << "-----------\n"
              << "compiler     : " << GetCompilerString() << "\n"
              << "c++ standard : " << __cplusplus << "\n"
              << "build type   : " << GetBuildType() << "\n"
              << "pointer size : " << pointer_bits.str() << "\n"
              << "cpu model    : " << GetCpuModel() << "\n"
              << "hw threads   : " << std::thread::hardware_concurrency()
              << "\n"
              << "eigen ver    : " << EIGEN_WORLD_VERSION << "."
              << EIGEN_MAJOR_VERSION << "." << EIGEN_MINOR_VERSION << "\n"
              << "eigen thrds  : " << Eigen::nbThreads() << "\n\n";
}

void PrintUsage(const char *program_name) {
    std::cout
        << "Usage: " << program_name << " [options]\n\n"
        << "Options:\n"
        << "  --state-size <n>   Number of state dimensions (default: 200)\n"
        << "  --steps <n>        Number of simulation timesteps per sample "
           "(default: 365)\n"
        << "  --warmup <n>       Warm-up iterations per repetition (default: "
           "5)\n"
        << "  --samples <n>      Timed samples per repetition (default: 25)\n"
        << "  --repetitions <n>  Independent repetitions (default: 3)\n"
        << "  --history-capture-interval <n>  Record every n timesteps "
           "(default: 1)\n"
        << "  --help             Show this help\n";
}

int ParsePositiveInt(const std::string &value, const std::string &name) {
    const long parsed = std::stol(value);
    if (parsed <= 0) {
        throw std::invalid_argument(name + " must be > 0");
    }
    return static_cast<int>(parsed);
}

std::size_t ParsePositiveSizeT(const std::string &value,
                               const std::string &name) {
    const long long parsed = std::stoll(value);
    if (parsed <= 0) {
        throw std::invalid_argument(name + " must be > 0");
    }
    return static_cast<std::size_t>(parsed);
}

BenchmarkConfig ParseArgs(int argc, char **argv) {
    BenchmarkConfig config;

    for (int i = 1; i < argc; ++i) {
        const std::string_view arg(argv[i]);

        auto require_value = [&](const std::string_view flag) -> std::string {
            if (i + 1 >= argc) {
                throw std::invalid_argument("Missing value for " +
                                            std::string(flag));
            }
            ++i;
            return std::string(argv[i]);
        };

        if (arg == "--help") {
            PrintUsage(argv[0]);
            std::exit(0);
        }
        if (arg == "--state-size") {
            config.state_size =
                ParsePositiveSizeT(require_value(arg), "--state-size");
            continue;
        }
        if (arg == "--steps") {
            config.steps = ParsePositiveInt(require_value(arg), "--steps");
            continue;
        }
        if (arg == "--warmup") {
            config.warmup_iterations =
                ParsePositiveInt(require_value(arg), "--warmup");
            continue;
        }
        if (arg == "--samples") {
            config.sample_iterations =
                ParsePositiveInt(require_value(arg), "--samples");
            continue;
        }
        if (arg == "--repetitions") {
            config.repetitions =
                ParsePositiveInt(require_value(arg), "--repetitions");
            continue;
        }
        if (arg == "--history-capture-interval") {
            config.history_capture_interval = ParsePositiveInt(
                require_value(arg), "--history-capture-interval");
            continue;
        }

        throw std::invalid_argument("Unknown argument: " + std::string(arg));
    }

    return config;
}

Eigen::MatrixXd MakeShiftMatrix(std::size_t n, double stay_probability,
                                int shift) {
    Eigen::MatrixXd matrix = Eigen::MatrixXd::Zero(
        static_cast<Eigen::Index>(n), static_cast<Eigen::Index>(n));

    for (std::size_t col = 0; col < n; ++col) {
        const std::size_t shifted = static_cast<std::size_t>(
            (static_cast<long long>(col) + shift + static_cast<long long>(n)) %
            static_cast<long long>(n));
        matrix(static_cast<Eigen::Index>(col), static_cast<Eigen::Index>(col)) =
            stay_probability;
        matrix(static_cast<Eigen::Index>(shifted),
               static_cast<Eigen::Index>(col)) = 1.0 - stay_probability;
    }
    return matrix;
}

Eigen::VectorXd MakeRateVector(std::size_t n, double base_rate,
                               double gradient) {
    Eigen::VectorXd v(static_cast<Eigen::Index>(n));
    for (std::size_t i = 0; i < n; ++i) {
        v(static_cast<Eigen::Index>(i)) =
            base_rate +
            gradient * static_cast<double>(i) / static_cast<double>(n);
    }
    return v;
}

std::unique_ptr<respond::Model> BuildModel(std::size_t state_size,
                                           int history_capture_interval,
                                           int final_timestep) {
    auto model = respond::Model::Create("benchmark_model", "console");
    model->SetHistoryCaptureInterval(history_capture_interval);
    model->SetFinalTimestep(final_timestep);

    auto behavior =
        respond::TransitionFactory::CreateTransition("behavior", "console");
    auto intervention =
        respond::TransitionFactory::CreateTransition("intervention", "console");
    auto overdose =
        respond::TransitionFactory::CreateTransition("overdose", "console");
    auto background = respond::TransitionFactory::CreateTransition(
        "background_death", "console");

    if (!behavior || !intervention || !overdose || !background) {
        throw std::runtime_error("Failed to create one or more transitions");
    }

    behavior->AddTransitionMatrix(MakeShiftMatrix(state_size, 0.985, 1));
    intervention->AddTransitionMatrix(MakeShiftMatrix(state_size, 0.990, -1));
    overdose->AddTransitionMatrix(MakeRateVector(state_size, 0.0020, 0.0005));
    overdose->AddTransitionMatrix(MakeRateVector(state_size, 0.0800, 0.0200));
    background->AddTransitionMatrix(MakeRateVector(state_size, 0.0008, 0.0004));

    model->AddTransition(behavior);
    model->AddTransition(intervention);
    model->AddTransition(overdose);
    model->AddTransition(background);

    return model;
}

TimedRunResult TimeOneSample(respond::Model &model,
                             const Eigen::VectorXd &initial_state, int steps) {
    model.SetState(initial_state);
    model.ClearHistories();
    model.SetFinalTimestep(steps);

    const auto start = Clock::now();
    for (int i = 0; i < steps; ++i) {
        model.RunTransitions();
    }
    const auto end = Clock::now();

    const double checksum = model.GetState().sum();
    std::size_t recorded_points = 0;
    const auto histories = model.GetHistories();
    const auto state_history = histories.find("state");
    if (state_history != histories.end()) {
        recorded_points = state_history->second.GetRecordedTimesteps().size();
    }
    DoNotOptimize(checksum);
    ClobberMemory();

    return {.elapsed_ns = Nanoseconds(end - start).count(),
            .checksum = checksum,
            .recorded_points = recorded_points};
}

Statistics ComputeStats(std::vector<double> samples_ns) {
    Statistics s;
    if (samples_ns.empty()) {
        return s;
    }

    std::sort(samples_ns.begin(), samples_ns.end());

    s.min_ns = samples_ns.front();
    s.max_ns = samples_ns.back();
    s.mean_ns = std::accumulate(samples_ns.begin(), samples_ns.end(), 0.0) /
                static_cast<double>(samples_ns.size());

    const std::size_t mid = samples_ns.size() / 2;
    if (samples_ns.size() % 2 == 0U) {
        s.median_ns = 0.5 * (samples_ns[mid - 1] + samples_ns[mid]);
    } else {
        s.median_ns = samples_ns[mid];
    }

    const std::size_t p95_idx = static_cast<std::size_t>(
        std::ceil(0.95 * static_cast<double>(samples_ns.size())) - 1.0);
    s.p95_ns = samples_ns[std::min(p95_idx, samples_ns.size() - 1)];

    double accum = 0.0;
    for (const double sample : samples_ns) {
        const double delta = sample - s.mean_ns;
        accum += delta * delta;
    }
    s.stddev_ns = std::sqrt(accum / static_cast<double>(samples_ns.size()));

    return s;
}

void PrintConfig(const BenchmarkConfig &config) {
    std::cout << "RESPOND Benchmark\n"
              << "-----------------\n"
              << "state_size   : " << config.state_size << "\n"
              << "steps        : " << config.steps << "\n"
              << "warmup       : " << config.warmup_iterations << "\n"
              << "samples      : " << config.sample_iterations << "\n"
              << "repetitions  : " << config.repetitions << "\n"
              << "hist every   : " << config.history_capture_interval << "\n\n";
}

void PrintStatsRow(const std::string &label, const Statistics &stats, int steps,
                   double checksum, std::size_t recorded_points) {
    const double mean_ms = ToMilliseconds(stats.mean_ns);
    const double median_ms = ToMilliseconds(stats.median_ns);
    const double p95_ms = ToMilliseconds(stats.p95_ns);
    const double min_ms = ToMilliseconds(stats.min_ns);
    const double max_ms = ToMilliseconds(stats.max_ns);
    const double stddev_ms = ToMilliseconds(stats.stddev_ns);
    const double ns_per_step = stats.mean_ns / static_cast<double>(steps);

    std::cout << std::left << std::setw(10) << label << std::right
              << std::setw(12) << std::fixed << std::setprecision(3) << mean_ms
              << std::setw(12) << median_ms << std::setw(12) << p95_ms
              << std::setw(12) << min_ms << std::setw(12) << max_ms
              << std::setw(12) << stddev_ms << std::setw(14)
              << std::setprecision(1) << ns_per_step << std::setw(12)
              << recorded_points << std::setw(14) << std::setprecision(4)
              << checksum << "\n";
}

} // namespace

int main(int argc, char **argv) {
    try {
        const BenchmarkConfig config = ParseArgs(argc, argv);
        PrintConfig(config);
        PrintEnvironmentMetadata();

        std::vector<double> all_samples_ns;
        all_samples_ns.reserve(
            static_cast<std::size_t>(config.repetitions) *
            static_cast<std::size_t>(config.sample_iterations));

        std::cout << std::left << std::setw(10) << "run" << std::right
                  << std::setw(12) << "mean_ms" << std::setw(12) << "p50_ms"
                  << std::setw(12) << "p95_ms" << std::setw(12) << "min_ms"
                  << std::setw(12) << "max_ms" << std::setw(12) << "std_ms"
                  << std::setw(14) << "ns/step" << std::setw(12) << "state_pts"
                  << std::setw(14) << "checksum"
                  << "\n";
        std::cout << std::string(122, '-') << "\n";

        double final_checksum = 0.0;
        std::size_t final_recorded_points = 0;

        for (int repetition = 0; repetition < config.repetitions;
             ++repetition) {
            auto model =
                BuildModel(config.state_size, config.history_capture_interval,
                           config.steps);

            Eigen::VectorXd initial_state = Eigen::VectorXd::Constant(
                static_cast<Eigen::Index>(config.state_size), 1'000.0);

            for (int i = 0; i < config.warmup_iterations; ++i) {
                const auto warmup =
                    TimeOneSample(*model, initial_state, config.steps);
                DoNotOptimize(warmup.checksum);
            }

            std::vector<double> sample_ns;
            sample_ns.reserve(
                static_cast<std::size_t>(config.sample_iterations));

            for (int i = 0; i < config.sample_iterations; ++i) {
                const auto sample =
                    TimeOneSample(*model, initial_state, config.steps);
                sample_ns.push_back(sample.elapsed_ns);
                all_samples_ns.push_back(sample.elapsed_ns);
                final_checksum = sample.checksum;
                final_recorded_points = sample.recorded_points;
            }

            const Statistics rep_stats = ComputeStats(sample_ns);
            PrintStatsRow("rep" + std::to_string(repetition + 1), rep_stats,
                          config.steps, final_checksum, final_recorded_points);
        }

        std::cout << std::string(122, '-') << "\n";
        const Statistics overall = ComputeStats(all_samples_ns);
        PrintStatsRow("overall", overall, config.steps, final_checksum,
                      final_recorded_points);

        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "Benchmark failed: " << ex.what() << "\n";
        return 1;
    }
}
