# CCTSim

CCTSim is a C++ application that builds upon the [CCTools Library](https://github.com/ThatTilux/CCTools) and the [RAT-Library](https://rat-gui.com/library.html) to provide simulation capabilities for canted-cosine-theta (CCT) magnet models defined in JSON. This tool enables users to perform parameter sweeps over input ranges for specified parameters of the magnet model and compute corresponding output parameters, facilitating data generation for in-depth analysis. CCTSim is compatible with CCT magnet models created using the [RAT-GUI](https://rat-gui.com/index.html) or the [RAT-Library](https://rat-gui.com/library.html).

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
Running the code yields a CSV with data describing the relationship between the pitch scaling of the inner CCT layer and the min/max z coordinate of the magnet.
The pitch scaling of a layer describes the spacing between windings, therefore also influencing the length of the magnet. The magnet is along the z-axis, thus the z coordinates decribe the length of the magnet.

From the kink in the `z_min_negated` line, we can determine at which pitch scaling the inner layer becomes longer than the outer layer on the one side of the magnet. From the kink in `z_max`, we can determine the same for the other side.

<div align="center">
  <img width="684" alt="Pitch scaling of inner layer vs. Min and max z coordinates" src="https://github.com/user-attachments/assets/681bd8b7-928e-4ad3-83d8-134150b498c8">
</div>



## Author

Ole Kuhlmann  
Email: [tim.ole.kuhlmann@cern.ch](mailto:tim.ole.kuhlmann@cern.ch)  
GitHub: [ThatTilux](https://github.com/ThatTilux)

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
