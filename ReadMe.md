# CLWorks a OpenCL Plugin for Unreal Engine
CLWorks provides an OpenCL integration plugin for Unreal Engine. Enabling low-level access to GPU compute capabilities from within UE Projects. It forms a bridge between OpenCL compute kernels and Unreal Engine data structures (i.e. UTexture, FRHIBuffer, etc.). Allowing the use of heterogenous compute across AMD, Intel, and NVIDIA hardware.

## Features
- Load/Compile and Execute OpenCL program at runtime.
- Transfer data to and from OpenCL buffers and images.
- Synchronize results into corresponding Unreal Engine UTextures types.
- Integrated Unit Test support using Unreal Engine's automation framework.
- Cross-platform design supporting Windows and Linux.

## Use Cases
- **Custom GPU Compute Shaders**: Bypass HLSL material limitations and graphics pipeline restrictions by using OpenCL programs directly.
- **Offline Processing**: Use the GPU for mesh generation, image processing, or terrain analysis.
- **Physics or Simulation Pipelines**: Run GPU parallel computations (e.g. fluid, cloth, or particle systems).
- **Scientific or Data-Driven UE Apps**: Visualize and process large datasets on the GPU.

## Installation
1) Clone into your Unreal Engine project `Plugins/` folder.
    - Example: git clone https://github.com/JSzajek/CLWorks.git Plugins/CLWorks
2) Generate the Visual Studio project files (if necessary).

## Testing
The plugin utilizes Unreal Engine automation framework's SPECs(). It is possible to execute those test in two ways:
1) Command line execution:
   - UnrealEditor-Cmd.exe <PROJECT_HERE.uproject> -ExecCmds="Automation RunTests CLWorks Unit Test" -unattended -nopause
2) Editor execution:
   - Open `Tools` -> `Session Frontend`. Navigate to the `Automation` tab.
   - Run the "CLWorks Unit Test" tests.


### Plugin Outline
#### OpenCL Structures:
 -

### Example Usage
#### Program 


#### Buffer Creation


#### Texture Creation


### Dependencies
 - OpenCL 1.2 or later (avaliable via CPU or GPU drivers).
 - Unreal Engine 5.0+.
 - C++20 compatible compiler.

### Future Roadmap
[] Texture2DArray Support
[] Texture3D Support
[] Blueprint Exposure
[] Benchmarking Framework
[] Asynchronous Data Transfer

### License
Licenses Under the **Apache 2.0** License.