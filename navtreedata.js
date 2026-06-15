/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "RESPOND", "index.html", [
    [ "Researching Effective Strategies to Prevent Opioid Use Disorder", "index.html", "index" ],
    [ "Why RESPOND?", "d9/d6c/md_docs_src_motivation.html", [
      [ "Costing and Utility", "d9/d6c/md_docs_src_motivation.html#autotoc_md9", null ],
      [ "Why Refactor?", "d9/d6c/md_docs_src_motivation.html#autotoc_md10", null ]
    ] ],
    [ "Installation", "d7/d63/md_docs_src_installation.html", [
      [ "Dependencies", "d7/d63/md_docs_src_installation.html#autotoc_md12", null ],
      [ "Installing From Packages", "d7/d63/md_docs_src_installation.html#autotoc_md13", null ],
      [ "Installing From Source", "d7/d63/md_docs_src_installation.html#autotoc_md14", [
        [ "Building without Packaging or Testing", "d7/d63/md_docs_src_installation.html#autotoc_md15", null ],
        [ "Testing", "d7/d63/md_docs_src_installation.html#autotoc_md16", null ],
        [ "Packaging", "d7/d63/md_docs_src_installation.html#autotoc_md17", null ]
      ] ],
      [ "Fetch Content", "d7/d63/md_docs_src_installation.html#autotoc_md18", null ]
    ] ],
    [ "Managing the Data", "d0/d1d/md_docs_src_data.html", [
      [ "Required Inputs", "d0/d1d/md_docs_src_data.html#autotoc_md20", [
        [ "sim.conf", "d0/d1d/md_docs_src_data.html#autotoc_md21", [
          [ "simulation", "d0/d1d/md_docs_src_data.html#autotoc_md22", null ],
          [ "state", "d0/d1d/md_docs_src_data.html#autotoc_md23", null ],
          [ "demographic", "d0/d1d/md_docs_src_data.html#autotoc_md24", null ],
          [ "cost", "d0/d1d/md_docs_src_data.html#autotoc_md25", null ],
          [ "output", "d0/d1d/md_docs_src_data.html#autotoc_md26", null ]
        ] ],
        [ "Tabular Data", "d0/d1d/md_docs_src_data.html#autotoc_md27", null ]
      ] ],
      [ "Cost Effectiveness Inputs", "d0/d1d/md_docs_src_data.html#autotoc_md28", [
        [ "File Breakdown", "d0/d1d/md_docs_src_data.html#autotoc_md29", [
          [ "healthcare_utilization_cost.csv", "d0/d1d/md_docs_src_data.html#autotoc_md30", null ],
          [ "overdose_cost.csv", "d0/d1d/md_docs_src_data.html#autotoc_md31", null ],
          [ "pharmaceutical_cost.csv", "d0/d1d/md_docs_src_data.html#autotoc_md32", null ],
          [ "setting_utility.csv", "d0/d1d/md_docs_src_data.html#autotoc_md33", null ],
          [ "treatment_utilization_cost.csv", "d0/d1d/md_docs_src_data.html#autotoc_md34", null ]
        ] ]
      ] ]
    ] ],
    [ "Running the Model", "dd/d4c/md_docs_src_run.html", [
      [ "Using RESPOND as a C++ Library", "dd/d4c/md_docs_src_run.html#autotoc_md36", [
        [ "Basic Example", "dd/d4c/md_docs_src_run.html#autotoc_md37", null ]
      ] ],
      [ "Using the Standalone Executable", "dd/d4c/md_docs_src_run.html#autotoc_md38", [
        [ "Command-Line Usage", "dd/d4c/md_docs_src_run.html#autotoc_md39", null ],
        [ "Arguments", "dd/d4c/md_docs_src_run.html#autotoc_md40", null ],
        [ "Examples", "dd/d4c/md_docs_src_run.html#autotoc_md41", null ],
        [ "Output", "dd/d4c/md_docs_src_run.html#autotoc_md42", null ]
      ] ],
      [ "Python Integration", "dd/d4c/md_docs_src_run.html#autotoc_md43", null ]
    ] ],
    [ "Under the Hood", "d6/df1/md_docs_src_math.html", [
      [ "Entering Cohort Transitions", "d6/df1/md_docs_src_math.html#autotoc_md45", null ],
      [ "Intervention Transitions", "d6/df1/md_docs_src_math.html#autotoc_md46", null ],
      [ "Behavior Transitions", "d6/df1/md_docs_src_math.html#autotoc_md47", null ],
      [ "Overdose Transitions", "d6/df1/md_docs_src_math.html#autotoc_md48", null ],
      [ "Background Mortality Transitions", "d6/df1/md_docs_src_math.html#autotoc_md49", null ]
    ] ],
    [ "Limitations", "d6/d3e/md_docs_src_limitations.html", [
      [ "Library", "d6/d3e/md_docs_src_limitations.html#autotoc_md51", null ],
      [ "Data", "d6/d3e/md_docs_src_limitations.html#autotoc_md52", null ]
    ] ],
    [ "FAQs", "d8/d51/md_docs_src_faq.html", null ],
    [ "Architecture and Design", "d2/d4f/md_docs_src_architecture.html", [
      [ "Design Philosophy", "d2/d4f/md_docs_src_architecture.html#autotoc_md55", null ],
      [ "Component Architecture", "d2/d4f/md_docs_src_architecture.html#autotoc_md56", null ],
      [ "Core Classes", "d2/d4f/md_docs_src_architecture.html#autotoc_md57", [
        [ "Model (Abstract Base Class)", "d2/d4f/md_docs_src_architecture.html#autotoc_md58", null ],
        [ "Simulation", "d2/d4f/md_docs_src_architecture.html#autotoc_md59", null ],
        [ "Transition (Abstract Base Class)", "d2/d4f/md_docs_src_architecture.html#autotoc_md60", null ],
        [ "History", "d2/d4f/md_docs_src_architecture.html#autotoc_md61", null ],
        [ "TransitionFactory", "d2/d4f/md_docs_src_architecture.html#autotoc_md62", null ]
      ] ],
      [ "Design Patterns", "d2/d4f/md_docs_src_architecture.html#autotoc_md63", [
        [ "Factory Pattern (TransitionFactory)", "d2/d4f/md_docs_src_architecture.html#autotoc_md64", null ],
        [ "Template Method Pattern (Model → Transitions)", "d2/d4f/md_docs_src_architecture.html#autotoc_md65", null ],
        [ "Strategy Pattern (Transitions)", "d2/d4f/md_docs_src_architecture.html#autotoc_md66", null ],
        [ "Object Pool / Clone Pattern", "d2/d4f/md_docs_src_architecture.html#autotoc_md67", null ]
      ] ],
      [ "Memory Management", "d2/d4f/md_docs_src_architecture.html#autotoc_md68", [
        [ "Unique Ownership (unique_ptr)", "d2/d4f/md_docs_src_architecture.html#autotoc_md69", null ],
        [ "Shared Ownership (None by default)", "d2/d4f/md_docs_src_architecture.html#autotoc_md70", null ],
        [ "Safety Mechanisms", "d2/d4f/md_docs_src_architecture.html#autotoc_md71", null ]
      ] ],
      [ "Extensibility Points", "d2/d4f/md_docs_src_architecture.html#autotoc_md72", [
        [ "Adding a New Transition Type", "d2/d4f/md_docs_src_architecture.html#autotoc_md73", null ],
        [ "Adding New History Types", "d2/d4f/md_docs_src_architecture.html#autotoc_md74", null ]
      ] ],
      [ "Dependencies", "d2/d4f/md_docs_src_architecture.html#autotoc_md75", [
        [ "External Libraries", "d2/d4f/md_docs_src_architecture.html#autotoc_md76", null ],
        [ "Internal Organization", "d2/d4f/md_docs_src_architecture.html#autotoc_md77", null ]
      ] ],
      [ "Threading and Concurrency", "d2/d4f/md_docs_src_architecture.html#autotoc_md78", null ],
      [ "Performance Considerations", "d2/d4f/md_docs_src_architecture.html#autotoc_md79", [
        [ "State Vector Operations", "d2/d4f/md_docs_src_architecture.html#autotoc_md80", null ],
        [ "Sparse History", "d2/d4f/md_docs_src_architecture.html#autotoc_md81", null ],
        [ "Transition Execution Order", "d2/d4f/md_docs_src_architecture.html#autotoc_md82", null ]
      ] ],
      [ "Validation and Error Handling", "d2/d4f/md_docs_src_architecture.html#autotoc_md83", [
        [ "Current Approach", "d2/d4f/md_docs_src_architecture.html#autotoc_md84", null ],
        [ "Improvements for Future Versions", "d2/d4f/md_docs_src_architecture.html#autotoc_md85", null ]
      ] ],
      [ "Testing Strategy", "d2/d4f/md_docs_src_architecture.html#autotoc_md86", [
        [ "Unit Tests", "d2/d4f/md_docs_src_architecture.html#autotoc_md87", null ],
        [ "Integration Tests", "d2/d4f/md_docs_src_architecture.html#autotoc_md88", null ],
        [ "Mock Objects", "d2/d4f/md_docs_src_architecture.html#autotoc_md89", null ]
      ] ],
      [ "Future Architectural Considerations", "d2/d4f/md_docs_src_architecture.html#autotoc_md90", null ]
    ] ],
    [ "C++ API Guide", "d2/deb/md_docs_src_api_guide.html", [
      [ "Overview", "d2/deb/md_docs_src_api_guide.html#autotoc_md93", null ],
      [ "Core Concepts", "d2/deb/md_docs_src_api_guide.html#autotoc_md94", [
        [ "State Vectors", "d2/deb/md_docs_src_api_guide.html#autotoc_md95", null ],
        [ "Transitions", "d2/deb/md_docs_src_api_guide.html#autotoc_md96", null ],
        [ "History Tracking", "d2/deb/md_docs_src_api_guide.html#autotoc_md97", null ]
      ] ],
      [ "Model Class", "d2/deb/md_docs_src_api_guide.html#autotoc_md98", [
        [ "Key Methods", "d2/deb/md_docs_src_api_guide.html#autotoc_md99", null ]
      ] ],
      [ "Simulation Class", "d2/deb/md_docs_src_api_guide.html#autotoc_md100", [
        [ "Key Methods", "d2/deb/md_docs_src_api_guide.html#autotoc_md101", null ]
      ] ],
      [ "History Class", "d2/deb/md_docs_src_api_guide.html#autotoc_md102", [
        [ "Key Methods", "d2/deb/md_docs_src_api_guide.html#autotoc_md103", null ]
      ] ],
      [ "Transition Class", "d2/deb/md_docs_src_api_guide.html#autotoc_md104", [
        [ "Supported Transition Types", "d2/deb/md_docs_src_api_guide.html#autotoc_md105", null ]
      ] ],
      [ "Logging Integration", "d2/deb/md_docs_src_api_guide.html#autotoc_md106", null ],
      [ "Complete Example", "d2/deb/md_docs_src_api_guide.html#autotoc_md107", null ],
      [ "Memory Management", "d2/deb/md_docs_src_api_guide.html#autotoc_md108", null ],
      [ "Best Practices", "d2/deb/md_docs_src_api_guide.html#autotoc_md109", null ],
      [ "Common Patterns", "d2/deb/md_docs_src_api_guide.html#autotoc_md110", [
        [ "Running Multiple Independent Simulations", "d2/deb/md_docs_src_api_guide.html#autotoc_md111", null ],
        [ "Resetting Model State", "d2/deb/md_docs_src_api_guide.html#autotoc_md112", null ],
        [ "Copying Simulations", "d2/deb/md_docs_src_api_guide.html#autotoc_md113", null ]
      ] ],
      [ "Parallel Execution with Shared Logging", "d2/deb/md_docs_src_api_guide.html#autotoc_md114", [
        [ "Basic Parallel Logging Setup", "d2/deb/md_docs_src_api_guide.html#autotoc_md115", null ],
        [ "Running Models in Parallel with Unified Logging", "d2/deb/md_docs_src_api_guide.html#autotoc_md116", null ],
        [ "Shared Logger Pattern Options", "d2/deb/md_docs_src_api_guide.html#autotoc_md117", null ],
        [ "Monitoring Shared Loggers", "d2/deb/md_docs_src_api_guide.html#autotoc_md118", null ],
        [ "Thread-Safe File Sink Management", "d2/deb/md_docs_src_api_guide.html#autotoc_md119", null ],
        [ "Best Practices for Parallel Logging", "d2/deb/md_docs_src_api_guide.html#autotoc_md120", null ],
        [ "Troubleshooting", "d2/deb/md_docs_src_api_guide.html#autotoc_md121", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", null ],
        [ "Functions", "functions_func.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"annotated.html"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';