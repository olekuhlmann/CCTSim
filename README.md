# CCTSim
CCTSim is a C++ application that builds upon the [CCTools](https://github.com/ThatTilux/CCTools) library and serves as a framework for systematic simulations of canted-cosine-theta (CCT) magnet models. Users can specify one or more input parameters of the CCT with corresponding parameter ranges and output parameters to be computed for every set of input parameters. The results are saved in a CSV for in-depth analysis.
CCTSim contains a set of pre-defined input parameters and output criteria. Any input and output can be added by creating a class that derives from `CCTSim::InputParamRangeInterface` or `CCTSim::OutputCriterionInterface`, respectively.

This project is part of the FCC-ee HTS4 research project at CERN.

## Documentation
The documentation of this application can be viewed [here](https://thattilux.github.io/CCTSim/).
Additionally, example code showcasing core functionalities is located at `examples/example.cpp`.

## Installation

### Prerequisites
**Note: This software was made for Linux.**

Follow the [Prerequisites section](https://github.com/ThatTilux/CCTools/blob/main/README.md#prerequisites) from CCTools to install all necessary dependencies.

### Building and Running CCTSim

To install and run this application, follow these steps. Make sure you have followed the **Prerequisites** section.

1. Clone this repository and build the software:
    ```sh
    git clone https://github.com/ThatTilux/CCTSim.git
    cd CCTSim
    mkdir build
    cd build
    cmake ..
    make
    ```

2. Run all tests:
    ```sh
    ./bin/cctsim_test
    ```

3. To run the example code:
    ```sh
    ./bin/cctsim_example
    ```

To then use the software, follow these steps.

1. Create the file `src/main.cpp`, copy the contents from `examples/example.cpp` and adjust to your needs.

2. Place the JSON magnet model file in the `data` directory.

3. Re-build and run the main file:
    ```sh
    cd build
    cmake ..
    make
    ./bin/cctsim_main
    ```

## Example
Some example code is located at `examples/example.cpp`. 
Running the code yields a CSV with data describing the relationship between the pitch scaling of the inner CCT layer and the min/max z coordinate of the given example magnet.
The pitch scaling of a layer describes the spacing between windings, thus also influencing the length of the magnet. The magnet is along the z-axis, thus the z coordinates describe the length of the magnet.

From the kink in the `z_min_negated` line, we can determine at which pitch scaling the inner layer becomes longer than the outer layer on the one side of the magnet. From the kink in `z_max`, we can determine the same for the other side.

<div align="center">
  <img width="684" alt="Pitch scaling of inner layer vs. Min and max z coordinates" src="https://github.com/user-attachments/assets/681bd8b7-928e-4ad3-83d8-134150b498c8">
</div>



## Author

Ole Kuhlmann  
Email: [ole.kuhlmann@rwth-aachen.de](mailto:ole.kuhlmann@rwth-aachen.de)  
GitHub: [olekuhlmann](https://github.com/olekuhlmann)

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
