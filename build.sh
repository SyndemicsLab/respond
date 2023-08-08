#!/usr/bin/bash -l

#
# All lines starting with "#" are comments
# All lines starting with "#$" are SGE qsub commands
# 

#  Specify a project to use (Required for BUMC projects).
#$ -P respond

#  Give this job a name
#$ -N respond_calibration

#  Join standard output and error to a single file
#$ -j y

#  Name the file where to redirect standard output and error
#$ -o respond_calibration.qlog

#  Request time needed for job to run (default: 12 hours)
#$ -l h_rt=12:00:00

#  Send an email when the job begins and when it ends running
#  (b = when job begins, a = if job aborts, e = when job ends)
#$ -m a

# Whom to send the email to
#$ -M matthew.carroll@bmc.org

SCC_HOSTS=(
    "scc1"
    "scc2"
    "geo"
    "scc4"
)

# ensure the "build/" directory exists
[[ -d "build/" ]] || mkdir "build/"

# use a subshell to enter the build folder and build RESPOND
(
    if [[ "${SCC_HOSTS[*]}" =~ $(hostname) ]]; then
	module load boost/1.79.0_gcc5+
	module load cmake/3.22.2
	module load eigen/3.4.0
	module load openmpi
	module load gcc/12.2.0
    fi
    cd "build/" || return
    rm -rf ./*
    mkdir outputs
    cmake .. "$@"
    # use another subshell to build the model
    (
	# determine the number of processing units available
	CORES="$(nproc --all)"
	# if CORES > 1 compile in parallel where possible
	([[ ! -z "$CORES" ]] && make -j"$CORES") || make
    )
    # run gtests if built
    # [[ -f "test/respondTest" ]] && test/respondTest

    taskstart=$(( (SGE_TASK_ID - 1) * 1000 + 1 ))
    tasksend=$(( (SGE_TASK_ID * 1000) + 1 ))

    src/respond "TestInput/" "$taskstart" "$tasksend"
)
