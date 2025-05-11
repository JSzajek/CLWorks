#include "Misc/AutomationTest.h"

#include "Interfaces/IPluginManager.h"

#include "CLWorksLib.h"

// Reference: https://minifloppy.it/posts/2024/automated-testing-specs-ue5/#writing-tests

BEGIN_DEFINE_SPEC(FCLUnitTestsSpecs, "CLWorks Unit Test",
				  EAutomationTestFlags::EditorContext | 
				  EAutomationTestFlags::CommandletContext |
				  EAutomationTestFlags::ProductFilter);

// Variables and functions defined here will end up being member of 
// the FCLUnitTestsSpecs class and will be accessible in the tests

OpenCL::Device mDefaultDevice;

FString ModuleDirectory = IPluginManager::Get().FindPlugin("CLWorks")->GetBaseDir();

FTimespan TestTimeout_S = FTimespan(0, 0, 20); /* 20 Seconds */

END_DEFINE_SPEC(FCLUnitTestsSpecs);

void FCLUnitTestsSpecs::Define()
{
	// Essentials Of The CLWorks (e.g.).
	Describe("Essentials", [this]()
	{
		It("(1) Device Enumeration", [this]()
		{
			OpenCL::Device test_device;

			TestTrue(TEXT("Invalid Device!"), test_device.Get() != nullptr);
		});

		It("(2) Context Creation", [this]()
		{
			OpenCL::Context context(mDefaultDevice);

			if (!TestTrue(TEXT("Failed Context Creation!"), context.Get() != nullptr))
				return;
		});

		It("(3) Command Queue Creation", [this]()
		{
			OpenCL::Context context(mDefaultDevice);

			if (!TestTrue(TEXT("Failed Context Creation!"), context.Get() != nullptr))
				return;

			OpenCL::CommandQueue queue(context, mDefaultDevice);
			TestTrue(TEXT("Failed Command Queue Creation!"), queue.Get() != nullptr);
		});
	});

	Describe("Kernel Setup", [this]()
	{
		It("(1) Kernel Compilation - String", [this]()
		{
			OpenCL::Context context(mDefaultDevice);

			const std::string moduleDirStr = std::string(TCHAR_TO_UTF8(*ModuleDirectory));
			OpenCL::Program program(mDefaultDevice, context);
			program.ReadFromString("__kernel void test() { }");

			TestTrue(TEXT("Invalid Program!"), program.Get() != nullptr);
		});

		It("(2) Kernel Compilation - File", [this]()
		{
			OpenCL::Context context(mDefaultDevice);

			const std::string moduleDirStr = std::string(TCHAR_TO_UTF8(*ModuleDirectory));
			OpenCL::Program program(mDefaultDevice, context);
			program.ReadFromFile(moduleDirStr + "/UnitTest/Shaders/add_vectors.cl");

			TestTrue(TEXT("Invalid Program!"), program.Get() != nullptr);
		});
		
		It("(3) Kernel Failure", [this]()
		{
			OpenCL::Context context(mDefaultDevice);

			OpenCL::Program program(mDefaultDevice, context);
			program.ReadFromString("__kernel void test() { }");

			if (!TestTrue(TEXT("Invalid Program!"), program.Get() != nullptr))
				return;

			AddExpectedErrorPlain(TEXT("Couldn't Create A Kernel!"));

			OpenCL::Kernel kernel(program, "foo");

			TestFalse(TEXT("Kernel Should Fail to Find Invalid Function!"), kernel.IsValid());
		});

		It("(4) Argument Setting", [this]()
		{
			OpenCL::Context context(mDefaultDevice);

			OpenCL::Program program(mDefaultDevice, context);
			program.ReadFromString("__kernel void test(float a, float b) { }");

			if (!TestTrue(TEXT("Invalid Program!"), program.Get() != nullptr))
				return;

			OpenCL::Kernel kernel(program, "test");

			if (!TestTrue(TEXT("Invalid Kernel!"), kernel.IsValid()))
				return;

			kernel.SetArgument(0, 2.0f);

			if (!TestTrue(TEXT("Failed to Set First Kernel Argument Program!"), kernel.IsValid()))
				return;

			kernel.SetArgument(1, 11.0f);

			TestTrue(TEXT("Failed to Set Second Kernel Argument Program!"), kernel.IsValid());
		});

		It("(5) Invalid Argument", [this]()
		{
			OpenCL::Context context(mDefaultDevice);

			OpenCL::Program program(mDefaultDevice, context);
			program.ReadFromString("__kernel void test(float a) { }");

			if (!TestTrue(TEXT("Invalid Program!"), program.Get() != nullptr))
				return;

			OpenCL::Kernel kernel(program, "test");
			if (!TestTrue(TEXT("Invalid Kernel!"), kernel.IsValid()))
				return;

			AddExpectedErrorPlain(TEXT("Couldn't Create Kernel Argument!"));

			kernel.SetArgument(3, 9.0f);

			TestFalse(TEXT("Set Invalid Kernel Argument!"), kernel.IsValid());
		});
	});

	Describe("Buffer Handling", [this]()
	{
		It("(1) Buffer Creation", [this]()
		{
			OpenCL::Context context(mDefaultDevice);

			size_t count = 10;
			std::vector<float> test_input(count, 0.0f);
			std::vector<float> test_output(count, 0.0f);
			std::vector<float> test_inout(count, 0.0f);


			OpenCL::Buffer buffer_input(context, test_input.data(), count * sizeof(float), OpenCL::AccessType::READ_ONLY);
			if (!TestNotNull(TEXT("Failed Read-Only Buffer Creation!"), buffer_input.Get()))
				return;

			OpenCL::Buffer buffer_output(context, test_output.data(), count * sizeof(float), OpenCL::AccessType::WRITE_ONLY);
			if (!TestNotNull(TEXT("Failed Write-Only Buffer Creation!"), buffer_output.Get()))
				return;

			OpenCL::Buffer buffer_inout(context, test_inout.data(), count * sizeof(float), OpenCL::AccessType::READ_WRITE);
			TestNotNull(TEXT("Failed Read-Write Buffer Creation!"), buffer_output.Get());
		});

		It("(2) Buffer Read", [this]()
		{
			OpenCL::Context context(mDefaultDevice);

			size_t count = 5;
			std::vector<float> input_data = { 30, 2, 45, 19, 54 };

			OpenCL::Buffer buffer_input(context, input_data.data(), count * sizeof(float), OpenCL::AccessType::READ_ONLY);
			if (!TestNotNull(TEXT("Failed Read-Only Buffer Creation!"), buffer_input.Get()))
				return;

			std::vector<float> target_output(count, 0.0f);

			OpenCL::CommandQueue queue(context, mDefaultDevice);
			queue.ReadBuffer(buffer_input, count * sizeof(float), target_output.data());

			for (size_t i = 0; i < count; ++i)
			{
				std::string msg = (std::to_string(target_output[i]) + " != " + std::to_string(input_data[i]));
				if (!TestTrue(FString(msg.c_str()), target_output[i] == input_data[i]))
					return;
			}
		});

		It("(3) Buffer Write", [this]()
		{
			OpenCL::Context context(mDefaultDevice);

			OpenCL::Program program(mDefaultDevice, context);
			program.ReadFromString("__kernel void double_data(__global float* data)\n" 
								   "{ int i = get_global_id(0); \n"
								   "data[i] = data[i] * 2; }");

			if (!TestTrue(TEXT("Invalid Program!"), program.Get() != nullptr))
				return;

			size_t count = 5;
			std::vector<float> input_data = { 30, 2, 45, 19, 54 };
			std::vector<float> target_output = { 60, 4, 90, 38, 108 };

			OpenCL::Buffer buffer_input(context, input_data.data(), count * sizeof(float), OpenCL::AccessType::READ_WRITE);
			if (!TestNotNull(TEXT("Failed Read-Write Buffer Creation!"), buffer_input.Get()))
				return;

			OpenCL::Kernel kernel(program, "double_data");
			OpenCL::CommandQueue queue(context, mDefaultDevice);

			kernel.SetArgument<cl_mem>(0, buffer_input);

			queue.EnqueueRange(kernel, 1, &count);

			std::vector<float> output_data(count, 0.0f);
			queue.ReadBuffer(buffer_input, count * sizeof(float), output_data.data());

			for (size_t i = 0; i < count; ++i)
			{
				std::string msg = (std::to_string(target_output[i]) + " != " + std::to_string(output_data[i]));
				if (!TestTrue(FString(msg.c_str()), target_output[i] == output_data[i]))
					return;
			}
		});

		It("(4) Multi-Buffer Access", [this]()
		{
			//TODO:: Implement
		});
	});

	Describe("Kernel Execution", [this]()
	{
		It("(1) Enqueue", [this]()
		{
			OpenCL::Context context(mDefaultDevice);

			OpenCL::Program program(mDefaultDevice, context);
			program.ReadFromString("__kernel void test(__global float* data) { }");

			if (!TestTrue(TEXT("Invalid Program!"), program.Get() != nullptr))
				return;

			size_t count = 5;
			std::vector<float> input_data(count, 3.0f);

			OpenCL::Buffer buffer_input(context, input_data.data(), count * sizeof(float), OpenCL::AccessType::READ_WRITE);
			if (!TestNotNull(TEXT("Failed Read-Write Buffer Creation!"), buffer_input.Get()))
				return;

			OpenCL::Kernel kernel(program, "test");
			OpenCL::CommandQueue queue(context, mDefaultDevice);

			kernel.SetArgument<cl_mem>(0, buffer_input);
			if (!TestTrue(TEXT("Couldn't Set Kernel Arguments!"), kernel.IsValid()))
				return;

			queue.EnqueueRange(kernel, 1, &count);
			TestTrue(TEXT("Couldn't Enqueue the Queue!"), queue.IsValid());
		});

		It("(2) Work Sizes", [this]()
		{
			OpenCL::Context context(mDefaultDevice);

			OpenCL::Program program(mDefaultDevice, context);
			program.ReadFromString("__kernel void triple_data(__global const float* data, __global float* result)\n" 
								   "{ int i = get_global_id(0); \n"
								   "result[i] = data[i] * 3; }");

			if (!TestTrue(TEXT("Invalid Program!"), program.Get() != nullptr))
				return;

			size_t count = 5;
			std::vector<float> input_data = { 30, 2, 45, 19, 54 };

			OpenCL::Buffer buffer_input(context, input_data.data(), count * sizeof(float), OpenCL::AccessType::READ_ONLY);
			OpenCL::Buffer buffer_output(context, input_data.data(), count * sizeof(float), OpenCL::AccessType::WRITE_ONLY);
			if (!TestNotNull(TEXT("Failed Read-Write Buffer Creation!"), buffer_input.Get()))
				return;

			OpenCL::Kernel kernel(program, "triple_data");
			OpenCL::CommandQueue queue(context, mDefaultDevice);

			kernel.SetArgument<cl_mem>(0, buffer_input);
			kernel.SetArgument<cl_mem>(1, buffer_output);

			// Partial Range --------------------------------------------------
			size_t range = 2;
			queue.EnqueueRange(kernel, 1, &range);
			if (!TestTrue(TEXT("Couldn't Enqueue the Queue!"), queue.IsValid()))
				return;

			std::vector<float> part_target_output = { 90, 6, 45, 19, 54 };

			std::vector<float> part_output_data(count, 0.0f);
			queue.ReadBuffer(buffer_output, range * sizeof(float), part_output_data.data());

			for (size_t i = 0; i < count; ++i)
			{
				if (i < range)
				{
					std::string msg = (std::to_string(part_target_output[i]) + " != " + std::to_string(part_output_data[i]));
					if (!TestTrue(FString(msg.c_str()), part_target_output[i] == part_output_data[i]))
						return;
				}
				else
				{
					if (!TestTrue("Values != 0", part_output_data[i] == 0))
						return;
				}
			}
			// ----------------------------------------------------------------

			// Full Range -----------------------------------------------------
			queue.EnqueueRange(kernel, 1, &count);
			if (!TestTrue(TEXT("Couldn't Enqueue the Queue!"), queue.IsValid()))
				return;

			std::vector<float> full_target_output = { 90, 6, 135, 57, 162 };

			std::vector<float> full_output_data(count, 0.0f);
			queue.ReadBuffer(buffer_output, count * sizeof(float), full_output_data.data());

			for (size_t i = 0; i < count; ++i)
			{
				std::string msg = (std::to_string(full_target_output[i]) + " != " + std::to_string(full_output_data[i]));
				if (!TestTrue(FString(msg.c_str()), full_target_output[i] == full_output_data[i]))
					return;
			}
			// ----------------------------------------------------------------
		});

		It("(3) Execution Synchronization", [this]()
		{
			//TODO:: Implement

		});
	});

	Describe("Textures", [this]()
	{
		It("(1) Texture2D Creation", [this]()
		{
			OpenCL::Context context(mDefaultDevice);

			OpenCL::Texture cltexture(context,
									  mDefaultDevice,
									  256, 
									  256, 
									  1,		
									  OpenCL::Texture::Format::RGBA8, 
									  OpenCL::Texture::Type::Texture2D);

			TestNotNull(TEXT("Failed Texture2D Creation!"), cltexture.Get());
		});

		It("(2) Texture2D Read Write", [this]()
		{
			OpenCL::Context context(mDefaultDevice);
			OpenCL::Program program(mDefaultDevice, context);

			OpenCL::Texture cltexture(context,
									  mDefaultDevice,
									  256, 
									  256, 
									  1,		
									  OpenCL::Texture::Format::RGBA8, 
									  OpenCL::Texture::Type::Texture2D,
									  OpenCL::AccessType::READ_WRITE);

			program.ReadFromString("__kernel void write_red_img(read_write image2d_t output)\n" 
								   "{ const int2 coord = (int2)(get_global_id(0), get_global_id(1)); \n"
								   "  write_imagef(output, coord, (float4)(1.0f, 0.0f, 0.0f, 1.0f)); }");

			OpenCL::Kernel kernel(program, "write_red_img");
			OpenCL::CommandQueue queue(context, mDefaultDevice);

			kernel.SetArgument(0, cltexture.Get());
			
			size_t global_work_size[2] = { 256, 256 };
			queue.EnqueueRange(kernel, 2, global_work_size);

			const size_t numPixels = cltexture.GetPixelCount();
			const size_t numChannels = cltexture.GetChannelCount();

			std::vector<uint8_t> output_data(numPixels * numChannels, 0);
			queue.ReadImageTo(cltexture, output_data.data());

			for (size_t i = 0; i < numPixels; i += numChannels)
			{
				const uint8_t r = output_data[i];
				const uint8_t g = output_data[i + 1];
				const uint8_t b = output_data[i + 2];
				const uint8_t a = output_data[i + 3];

				if (!TestTrue(TEXT("Pixel Isn't Red!"), r == 255 && g == 0 && b == 0 && a == 255))
					return;
			}
		});
	});

	Describe("Programs", [this]()
	{
		It("(1) Vector Buffer Addition", [this]()
		{
			OpenCL::Device device;

			OpenCL::ContextProperties contextProperties;
			OpenCL::Context context(device, contextProperties);

			const std::string moduleDirStr = std::string(TCHAR_TO_UTF8(*ModuleDirectory));
			OpenCL::Program program(device, context);
			program.ReadFromFile(moduleDirStr + "/UnitTest/Shaders/add_vectors.cl");

			if (!TestTrue(TEXT("Invalid Program!"), program.Get() != nullptr))
				return;

			OpenCL::Kernel kernel(program, "vectors_add");

			OpenCL::CommandQueue queue(context, device);

			const size_t numValues = 6;
			const size_t local_size = 6;
			std::vector<float> vectorA = { 20, 35, 1, 2, 6, 2 };
			std::vector<float> vectorB = { 49, 3, 58, 34, 5, 9 };
			std::vector<float> output(numValues, 0.0f);

			// Perform Kernel Execution ---------------------------------------
			if (!TestTrue(TEXT("Invalid Vector Sizes!"), vectorA.size() == vectorB.size()))
				return;

			const size_t dataSize = numValues * sizeof(float);
			OpenCL::Buffer bufferA(context, vectorA.data(), dataSize, OpenCL::AccessType::READ_ONLY);
			OpenCL::Buffer bufferB(context, vectorB.data(), dataSize, OpenCL::AccessType::READ_ONLY);
			OpenCL::Buffer outputBuffer(context, output.data(), dataSize, OpenCL::AccessType::WRITE_ONLY);

			kernel.SetArgument<cl_mem>(0, bufferA);
			kernel.SetArgument<cl_mem>(1, bufferB);
			kernel.SetArgument<cl_mem>(2, outputBuffer);

			if (!TestTrue(TEXT("Couldn't Set Kernel Arguments!"), kernel.IsValid()))
				return;

			queue.EnqueueRange(kernel, 1, &numValues);

			if (!TestTrue(TEXT("Couldn't Enqueue the Kernel!"), queue.IsValid()))
				return;

			queue.ReadBuffer(outputBuffer, numValues * sizeof(float), output.data());

			if (!TestTrue(TEXT("Couldn't Read the Buffer!"), queue.IsValid()))
				return;

			// ----------------------------------------------------------------

			// Perform Manual Addition ----------------------------------------
			std::vector<float> checkOutput(numValues, 0.0f);
			for (size_t i = 0; i < numValues; ++i)
				checkOutput[i] = vectorA[i] + vectorB[i];
			// ----------------------------------------------------------------

			for (size_t i = 0; i < numValues; ++i)
			{
				std::string msg = (std::to_string(checkOutput[i]) + " != " + std::to_string(output[i]));
				if (!TestTrue(FString(msg.c_str()), checkOutput[i] == output[i]))
					return;
			}
		});
	});
}
