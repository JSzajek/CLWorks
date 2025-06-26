#include "Misc/AutomationTest.h"

#include "Interfaces/IPluginManager.h"

#include "CLWorksLib.h"

#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2DArray.h"

#include "Private/UnitTests/TestUWorld.h"
#include "Kismet/KismetRenderingLibrary.h"

// Reference: https://minifloppy.it/posts/2024/automated-testing-specs-ue5/#writing-tests

BEGIN_DEFINE_SPEC(FCLUnitTestsSpecs, "CLWorks Unit Test",
				  EAutomationTestFlags::EditorContext | 
				  EAutomationTestFlags::CommandletContext |
				  EAutomationTestFlags::ProductFilter);

// Variables and functions defined here will end up being member of
// the FCLUnitTestsSpecs class and will be accessible in the tests

TUniquePtr<FTestUWorld> TestWorld = nullptr;

OpenCL::DevicePtr mpDefaultDevice = OpenCL::MakeDevice();

int32 mDefaultUTextureWidth = 256;
int32 mDefaultUTextureHeight = 256;
int32 mDefaultUTextureArraySlices = 4;

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
			OpenCL::ContextPtr context = MakeContext(mpDefaultDevice);

			if (!TestTrue(TEXT("Failed Context Creation!"), context->Get() != nullptr))
				return;
		});

		It("(3) Command Queue Creation", [this]()
		{
			OpenCL::ContextPtr context = MakeContext(mpDefaultDevice);

			if (!TestTrue(TEXT("Failed Context Creation!"), context->Get() != nullptr))
				return;

			OpenCL::CommandQueue queue(context, mpDefaultDevice);
			TestTrue(TEXT("Failed Command Queue Creation!"), queue.Get() != nullptr);
		});
	});

	Describe("Kernel Setup", [this]()
	{
		It("(1) Kernel Compilation - String", [this]()
		{
			OpenCL::ContextPtr context = MakeContext(mpDefaultDevice);

			const std::string moduleDirStr = std::string(TCHAR_TO_UTF8(*ModuleDirectory));
			OpenCL::Program program(context, mpDefaultDevice);
			program.ReadFromString("__kernel void test() { }");

			TestTrue(TEXT("Invalid Program!"), program.Get() != nullptr);
		});

		It("(2) Kernel Compilation - File", [this]()
		{
			OpenCL::ContextPtr context = MakeContext(mpDefaultDevice);

			const std::string moduleDirStr = std::string(TCHAR_TO_UTF8(*ModuleDirectory));
			OpenCL::Program program(context, mpDefaultDevice);
			program.ReadFromFile(moduleDirStr + "/UnitTest/Shaders/add_vectors.cl");

			TestTrue(TEXT("Invalid Program!"), program.Get() != nullptr);
		});
		
		It("(3) Kernel Failure", [this]()
		{
			OpenCL::ContextPtr context = MakeContext(mpDefaultDevice);

			OpenCL::Program program(context, mpDefaultDevice);
			program.ReadFromString("__kernel void test() { }");

			if (!TestTrue(TEXT("Invalid Program!"), program.Get() != nullptr))
				return;

			AddExpectedErrorPlain(TEXT("Couldn't Create A Kernel!"));

			OpenCL::Kernel kernel(program, "foo");

			TestFalse(TEXT("Kernel Should Fail to Find Invalid Function!"), kernel.IsValid());
		});

		It("(4) Argument Setting", [this]()
		{
			OpenCL::ContextPtr context = MakeContext(mpDefaultDevice);

			OpenCL::Program program(context, mpDefaultDevice);
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
			OpenCL::ContextPtr context = MakeContext(mpDefaultDevice);

			OpenCL::Program program(context, mpDefaultDevice);
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
			OpenCL::ContextPtr context = MakeContext(mpDefaultDevice);

			size_t count = 10;
			std::vector<float> test_input(count, 0.0f);
			std::vector<float> test_inout(count, 0.0f);


			OpenCL::Buffer buffer_output(mpDefaultDevice, context, test_input.data(), count * sizeof(float), OpenCL::AccessType::WRITE_ONLY, OpenCL::MemoryStrategy::COPY_ONCE);
			if (!TestTrue(TEXT("Failed Write-Only Buffer Creation!"), buffer_output.IsValid()))
				return;

			OpenCL::Buffer buffer_input(mpDefaultDevice, context, nullptr, count * sizeof(float), OpenCL::AccessType::READ_ONLY, OpenCL::MemoryStrategy::STREAM);
			if (!TestTrue(TEXT("Failed Read-Only Buffer Creation!"), buffer_input.IsValid()))
				return;

			OpenCL::Buffer buffer_inout(mpDefaultDevice, context, test_inout.data(), count * sizeof(float), OpenCL::AccessType::READ_WRITE, OpenCL::MemoryStrategy::STREAM);
			TestTrue(TEXT("Failed Read-Write Buffer Creation!"), buffer_inout.IsValid());
		});

		It("(2) Buffer Read", [this]()
		{
			OpenCL::ContextPtr context = MakeContext(mpDefaultDevice);

			size_t count = 5;
			std::vector<float> input_data = { 30, 2, 45, 19, 54 };

			OpenCL::Buffer buffer_input(mpDefaultDevice, context, input_data.data(), count * sizeof(float), OpenCL::AccessType::READ_WRITE, OpenCL::MemoryStrategy::STREAM);
			if (!TestTrue(TEXT("Failed Read-Only Buffer Creation!"), buffer_input.IsValid()))
				return;

			std::vector<float> target_output(count, 0.0f);

			OpenCL::CommandQueue queue(context, mpDefaultDevice);
			buffer_input.Fetch(queue, target_output.data(), count * sizeof(float));

			for (size_t i = 0; i < count; ++i)
			{
				std::string msg = (std::to_string(target_output[i]) + " != " + std::to_string(input_data[i]));
				if (!TestTrue(FString(msg.c_str()), target_output[i] == input_data[i]))
					return;
			}
		});

		It("(3) Buffer Read & Write", [this]()
		{
			OpenCL::ContextPtr context = MakeContext(mpDefaultDevice);

			OpenCL::Program program(context, mpDefaultDevice);
			program.ReadFromString("__kernel void double_data(__global float* data)\n" 
								   "{ int i = get_global_id(0); \n"
								   "data[i] = data[i] * 2; }");

			if (!TestTrue(TEXT("Invalid Program!"), program.Get() != nullptr))
				return;

			size_t count = 5;
			std::vector<float> input_data = { 30, 2, 45, 19, 54 };
			std::vector<float> target_output = { 60, 4, 90, 38, 108 };

			OpenCL::Buffer buffer_input(mpDefaultDevice, context, input_data.data(), count * sizeof(float), OpenCL::AccessType::READ_WRITE, OpenCL::MemoryStrategy::STREAM);
			if (!TestTrue(TEXT("Failed Read-Write Buffer Creation!"), buffer_input.IsValid()))
				return;

			OpenCL::Kernel kernel(program, "double_data");
			OpenCL::CommandQueue queue(context, mpDefaultDevice);

			kernel.SetArgument<OpenCL::Buffer>(0, buffer_input);

			queue.EnqueueRange(kernel, 1, &count);

			std::vector<float> output_data(count, 0.0f);
			buffer_input.Fetch(queue, output_data.data(), count * sizeof(float));

			for (size_t i = 0; i < count; ++i)
			{
				std::string msg = (std::to_string(target_output[i]) + " != " + std::to_string(output_data[i]));
				if (!TestTrue(FString(msg.c_str()), target_output[i] == output_data[i]))
					return;
			}
		});

		It("(4) Zero Copy Buffers", [this]()
		{
			OpenCL::ContextPtr context = MakeContext(mpDefaultDevice);

			OpenCL::Program program(context, mpDefaultDevice);
			program.ReadFromString("__kernel void half_data(__global float* data)\n" 
								   "{ int i = get_global_id(0); \n"
								   "data[i] = data[i] * 0.5f; }");

			if (!TestTrue(TEXT("Invalid Program!"), program.Get() != nullptr))
				return;

			size_t count = 5;
			std::vector<float> input_data = { 30, 2, 45, 19, 54 };
			std::vector<float> target_output = { 15, 1, 22.5, 9.5, 27 };

			OpenCL::Buffer buffer_input(mpDefaultDevice, context, input_data.data(), count * sizeof(float), OpenCL::AccessType::READ_WRITE, OpenCL::MemoryStrategy::ZERO_COPY);
			if (!TestTrue(TEXT("Failed Read-Write Buffer Creation!"), buffer_input.IsValid()))
				return;

			OpenCL::Kernel kernel(program, "half_data");
			OpenCL::CommandQueue queue(context, mpDefaultDevice);

			kernel.SetArgument<OpenCL::Buffer>(0, buffer_input);

			queue.EnqueueRange(kernel, 1, &count);

			std::vector<float> output_data(count, 0.0f);
			buffer_input.Fetch(queue, output_data.data(), count * sizeof(float));

			for (size_t i = 0; i < count; ++i)
			{
				std::string msg = (std::to_string(target_output[i]) + " != " + std::to_string(output_data[i]));
				if (!TestTrue(FString(msg.c_str()), target_output[i] == output_data[i]))
					return;
			}
		});

		It("(5) Multi-Buffer Access", [this]()
		{
			//TODO:: Implement
		});
	});

	Describe("Kernel Execution", [this]()
	{
		It("(1) Enqueue", [this]()
		{
			OpenCL::ContextPtr context = MakeContext(mpDefaultDevice);

			OpenCL::Program program(context, mpDefaultDevice);
			program.ReadFromString("__kernel void test(__global float* data) { }");

			if (!TestTrue(TEXT("Invalid Program!"), program.Get() != nullptr))
				return;

			size_t count = 5;
			std::vector<float> input_data(count, 3.0f);

			OpenCL::Buffer buffer_input(mpDefaultDevice, context, input_data.data(), count * sizeof(float), OpenCL::AccessType::READ_WRITE, OpenCL::MemoryStrategy::STREAM);
			if (!TestNotNull(TEXT("Failed Read-Write Buffer Creation!"), buffer_input.Get()))
				return;

			OpenCL::Kernel kernel(program, "test");
			OpenCL::CommandQueue queue(context, mpDefaultDevice);

			kernel.SetArgument<OpenCL::Buffer>(0, buffer_input);
			if (!TestTrue(TEXT("Couldn't Set Kernel Arguments!"), kernel.IsValid()))
				return;

			queue.EnqueueRange(kernel, 1, &count);
			TestTrue(TEXT("Couldn't Enqueue the Queue!"), queue.IsValid());
		});

		It("(2) Work Sizes", [this]()
		{
			OpenCL::ContextPtr context = MakeContext(mpDefaultDevice);

			OpenCL::Program program(context, mpDefaultDevice);
			program.ReadFromString("__kernel void triple_data(__global const float* data, __global float* result)\n" 
								   "{ int i = get_global_id(0); \n"
								   "result[i] = data[i] * 3; }");

			if (!TestTrue(TEXT("Invalid Program!"), program.Get() != nullptr))
				return;

			size_t count = 5;
			std::vector<float> input_data = { 30, 2, 45, 19, 54 };

			OpenCL::Buffer buffer_input(mpDefaultDevice, context, input_data.data(), count * sizeof(float), OpenCL::AccessType::WRITE_ONLY, OpenCL::MemoryStrategy::COPY_ONCE);
			OpenCL::Buffer buffer_output(mpDefaultDevice, context, nullptr, count * sizeof(float), OpenCL::AccessType::READ_ONLY, OpenCL::MemoryStrategy::STREAM);
			if (!TestNotNull(TEXT("Failed Read-Write Buffer Creation!"), buffer_input.Get()))
				return;

			OpenCL::Kernel kernel(program, "triple_data");
			OpenCL::CommandQueue queue(context, mpDefaultDevice);

			kernel.SetArgument<OpenCL::Buffer>(0, buffer_input);
			kernel.SetArgument<OpenCL::Buffer>(1, buffer_output);

			// Partial Range --------------------------------------------------
			size_t range = 2;
			queue.EnqueueRange(kernel, 1, &range);
			if (!TestTrue(TEXT("Couldn't Enqueue the Queue!"), queue.IsValid()))
				return;

			std::vector<float> part_target_output = { 90, 6, 45, 19, 54 };

			std::vector<float> part_output_data(count, 0.0f);
			buffer_output.Fetch(queue, part_output_data.data(), range * sizeof(float));

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
			buffer_output.Fetch(queue, full_output_data.data(), count * sizeof(float));

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
			OpenCL::ContextPtr context = MakeContext(mpDefaultDevice);

			OpenCL::Image cltexture(context,
								    mpDefaultDevice,
								    256, 
								    256, 
								    1,		
								    OpenCL::Image::Format::RGBA8, 
								    OpenCL::Image::Type::Texture2D);

			TestNotNull(TEXT("Failed Texture2D Creation!"), cltexture.Get());
		});

		It("(2) Texture2D Read Write", [this]()
		{
			OpenCL::ContextPtr context = MakeContext(mpDefaultDevice);
			OpenCL::Program program(context, mpDefaultDevice);

			OpenCL::Image cltexture(context,
									  mpDefaultDevice,
									  256, 
									  256, 
									  1,		
									  OpenCL::Image::Format::RGBA8, 
									  OpenCL::Image::Type::Texture2D,
									  OpenCL::AccessType::READ_WRITE);

			program.ReadFromString("__kernel void write_red_img(read_write image2d_t output)\n" 
								   "{ const int2 coord = (int2)(get_global_id(0), get_global_id(1)); \n"
								   "  write_imagef(output, coord, (float4)(1.0f, 0.0f, 0.0f, 1.0f)); }");

			OpenCL::Kernel kernel(program, "write_red_img");
			OpenCL::CommandQueue queue(context, mpDefaultDevice);

			kernel.SetArgument(0, cltexture.Get());
			
			size_t global_work_size[2] = { 256, 256 };
			queue.EnqueueRange(kernel, 2, global_work_size);

			const size_t numPixels = cltexture.GetPixelCount();
			const size_t numChannels = cltexture.GetChannelCount();

			std::vector<uint8_t> output_data(numPixels * numChannels, 0);
			cltexture.Fetch(queue, output_data.data());

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

		It("(3) Texture2D UE", [this]()
		{
			OpenCL::ContextPtr context = MakeContext(mpDefaultDevice);
			OpenCL::CommandQueue queue(context, mpDefaultDevice);

			OpenCL::Image cltexture(context,
									  mpDefaultDevice,
									  256, 
									  256, 
									  1,		
									  OpenCL::Image::Format::RGBA8, 
									  OpenCL::Image::Type::Texture2D);

			if (!TestNotNull(TEXT("Failed Texture2D Creation!"), cltexture.Get()))
				return;

			// Create a texture in UE
			TObjectPtr<UTexture2D> texture = cltexture.CreateUTexture2D(queue);

			TestNotNull(TEXT("Failed Texture2D Creation!"), texture.Get());

			texture->ConditionalBeginDestroy();
		});
	});

	Describe("UE Textures", [this]()
	{
		It("(1) UTexture Creation", [this]()
		{
			OpenCL::ContextPtr context = MakeContext(mpDefaultDevice);

			OpenCL::Program program(context, mpDefaultDevice);
			OpenCL::CommandQueue queue(context, mpDefaultDevice);

			const auto UTextureCreationTest = [&](OpenCL::Image::Format format)
			{
				OpenCL::Image cltexture(context,
										mpDefaultDevice,
										mDefaultUTextureWidth,
										mDefaultUTextureHeight,
										1,
										format,
										OpenCL::Image::Type::Texture2D);

				const std::string failedCLMsg = "Failed OpenCL Texture2D Creation! " + std::to_string(format);
				if (!TestNotNull(FString(failedCLMsg.c_str()), cltexture.Get()))
					return;

				UTexture2D* utexture = cltexture.CreateUTexture2D(queue, true, true);

				const std::string failedUEMsg = "Failed UTexture2D Creation! " + std::to_string(format);
				if (TestNotNull(FString(failedUEMsg.c_str()), utexture))
					utexture->ConditionalBeginDestroy();
			};


			// Test UTexture2D Formats --------------------
			
			UTextureCreationTest(OpenCL::Image::Format::R8);
			UTextureCreationTest(OpenCL::Image::Format::RG8);
			UTextureCreationTest(OpenCL::Image::Format::RGBA8);

			UTextureCreationTest(OpenCL::Image::Format::R32U);
			UTextureCreationTest(OpenCL::Image::Format::RG32U);
			UTextureCreationTest(OpenCL::Image::Format::RGBA32U);

			UTextureCreationTest(OpenCL::Image::Format::R32S);

			if (mpDefaultDevice->IsExtensionSupported("cl_khr_fp16"))
			{
				UTextureCreationTest(OpenCL::Image::Format::R16F);
				UTextureCreationTest(OpenCL::Image::Format::RG16F);
				UTextureCreationTest(OpenCL::Image::Format::RGBA16F);
			}

			UTextureCreationTest(OpenCL::Image::Format::R32F);
			UTextureCreationTest(OpenCL::Image::Format::RG32F);
			UTextureCreationTest(OpenCL::Image::Format::RGBA32F);

			// --------------------------------------------
		});

		It("(2) UTexture2DArray Creation", [this]()
		{
			OpenCL::ContextPtr context = MakeContext(mpDefaultDevice);

			OpenCL::Program program(context, mpDefaultDevice);
			OpenCL::CommandQueue queue(context, mpDefaultDevice);

			const auto UTexture2DArrayCreationTest = [&](OpenCL::Image::Format format)
			{
				OpenCL::Image cltexture(context,
										mpDefaultDevice,
										mDefaultUTextureWidth,
										mDefaultUTextureHeight,
										mDefaultUTextureArraySlices,
										format,
										OpenCL::Image::Type::Texture2DArray);

				const std::string failedCLMsg = "Failed OpenCL Texture2DArray Creation! " + std::to_string(format);
				if (!TestNotNull(FString(failedCLMsg.c_str()), cltexture.Get()))
					return;

				UTexture2DArray* utexture = cltexture.CreateUTexture2DArray(queue, true, true);

				const std::string failedUEMsg = "Failed UTexture2DArray Creation! " + std::to_string(format);
				if (TestNotNull(FString(failedUEMsg.c_str()), utexture))
					utexture->ConditionalBeginDestroy();
			};


			// Test UTexture2DArray Formats ---------------

			UTexture2DArrayCreationTest(OpenCL::Image::Format::R8);
			UTexture2DArrayCreationTest(OpenCL::Image::Format::RG8);
			UTexture2DArrayCreationTest(OpenCL::Image::Format::RGBA8);

			UTexture2DArrayCreationTest(OpenCL::Image::Format::R32U);
			UTexture2DArrayCreationTest(OpenCL::Image::Format::RG32U);
			UTexture2DArrayCreationTest(OpenCL::Image::Format::RGBA32U);

			UTexture2DArrayCreationTest(OpenCL::Image::Format::R32S);

			if (mpDefaultDevice->IsExtensionSupported("cl_khr_fp16"))
			{
				UTexture2DArrayCreationTest(OpenCL::Image::Format::R16F);
				UTexture2DArrayCreationTest(OpenCL::Image::Format::RG16F);
				UTexture2DArrayCreationTest(OpenCL::Image::Format::RGBA16F);
			}

			UTexture2DArrayCreationTest(OpenCL::Image::Format::R32F);
			UTexture2DArrayCreationTest(OpenCL::Image::Format::RG32F);
			UTexture2DArrayCreationTest(OpenCL::Image::Format::RGBA32F);

			// --------------------------------------------

		});

		It("(3) UTexture2D Read/Writes", [this]()
		{
		});

		LatentIt("(4) UTextureRenderTarget2D Read/Writes", EAsyncExecution::ThreadPool, TestTimeout_S, [this](const FDoneDelegate& Done)
		{
			UTextureRenderTarget2D* rt = nullptr;
			FGraphEventRef loadRT = FFunctionGraphTask::CreateAndDispatchWhenReady
			([Done, this]()
			{
				OpenCL::ContextPtr context = MakeContext(mpDefaultDevice);

				OpenCL::Program program(context, mpDefaultDevice);
				OpenCL::CommandQueue queue(context, mpDefaultDevice);

				OpenCL::Image::Format testFormats[] = 
				{
					OpenCL::Image::Format::RGBA8,
					OpenCL::Image::Format::RGBA16F
				};

				for (OpenCL::Image::Format format : testFormats)
				{
					if (format == OpenCL::Image::RGBA16F)
					{
						// Check for device support - skip otherwise...
						if (!mpDefaultDevice->IsExtensionSupported("cl_khr_fp16"))
							continue;
					}

					OpenCL::Image cltexture(context,
											mpDefaultDevice,
											mDefaultUTextureWidth,
											mDefaultUTextureHeight,
											1,
											format,
											OpenCL::Image::Type::Texture2D);

					const std::string failedCLMsg = "Failed OpenCL Texture2D Creation! " + std::to_string(format);
					if (!TestNotNull(FString(failedCLMsg.c_str()), cltexture.Get()))
					{
						Done.Execute();
						return;
					}

				
					if (format == OpenCL::Image::Format::RGBA16F)
					{
						program.ReadFromString("#pragma OPENCL EXTENSION cl_khr_fp16 : enable\n"
											   "__kernel void write_color_img(read_write image2d_t output)\n" 
											   "{ const int2 coord = (int2)(get_global_id(0), get_global_id(1)); \n"
											   "  const half4 color = (half4)(0.5f, 0.5f, 0.5f, 1.0f); \n"
											   "  write_imageh(output, coord, color); }");
					}
					else
					{
						program.ReadFromString("__kernel void write_color_img(read_write image2d_t output)\n" 
											   "{ const int2 coord = (int2)(get_global_id(0), get_global_id(1)); \n"
											   "  const float4 color = (float4)(0.0f, 1.0f, 0.0f, 1.0f); \n"
											   "  write_imagef(output, coord, color); }");
					}

					OpenCL::Kernel kernel(program, "write_color_img");

					kernel.SetArgument(0, cltexture.Get());

					size_t global_work_size[2] = { mDefaultUTextureWidth, mDefaultUTextureHeight };
					queue.EnqueueRange(kernel, 2, global_work_size);

					queue.WaitForFinish();

					ETextureRenderTargetFormat pixFormat = ETextureRenderTargetFormat::RTF_R8;
					if (format == OpenCL::Image::Format::RGBA8)
						pixFormat = ETextureRenderTargetFormat::RTF_RGBA8;
					else if (format == OpenCL::Image::Format::RGBA16F)
						pixFormat = ETextureRenderTargetFormat::RTF_RGBA16f;

					UTextureRenderTarget2D* rt = NewObject<UTextureRenderTarget2D>(GetTransientPackage(),
																				   NAME_None,
																				   RF_Transient);

					rt->RenderTargetFormat = pixFormat;
					rt->ClearColor = FLinearColor::Transparent;
					rt->bAutoGenerateMips = false;
					rt->bCanCreateUAV = true;
					rt->InitAutoFormat(mDefaultUTextureWidth, mDefaultUTextureHeight);
					rt->UpdateResourceImmediate(true);

					rt->AddToRoot();
				
					if (!TestNotNull("Failed to Create UTextureRenderTarget2D", rt))
					{
						Done.Execute();
						return;
					}

					
					bool res = cltexture.UploadToUTextureRenderTarget2D(rt, queue, false, [rt, Done, this]()
					{
						TUniquePtr<FTestUWorld> tempWorld = MakeUnique<FTestUWorld>();

						FColor rt_color = UKismetRenderingLibrary::ReadRenderTargetPixel(tempWorld->GetWorld(), rt, 0, 0);

						TestTrue(TEXT("Incorrect Color In Render Target2D!"), rt_color == FColor::Green);

						rt->RemoveFromRoot();
						rt->ConditionalBeginDestroy();

						tempWorld.Reset();

						Done.Execute();
					});

					TestTrue(TEXT("Failed Upload Into Render Target2D!"), res);
				}

			}, TStatId(), nullptr, ENamedThreads::GameThread);
		});
	});

	Describe("Programs", [this]()
	{
		It("(1) Vector Buffer Addition", [this]()
		{
			OpenCL::ContextPtr context = MakeContext(mpDefaultDevice);

			const std::string moduleDirStr = std::string(TCHAR_TO_UTF8(*ModuleDirectory));
			OpenCL::Program program(context, mpDefaultDevice);
			program.ReadFromFile(moduleDirStr + "/UnitTest/Shaders/add_vectors.cl");

			if (!TestTrue(TEXT("Invalid Program!"), program.Get() != nullptr))
				return;

			OpenCL::Kernel kernel(program, "vectors_add");

			OpenCL::CommandQueue queue(context, mpDefaultDevice);

			const size_t numValues = 6;
			const size_t local_size = 6;
			std::vector<float> vectorA = { 20, 35, 1, 2, 6, 2 };
			std::vector<float> vectorB = { 49, 3, 58, 34, 5, 9 };
			std::vector<float> output(numValues, 0.0f);

			// Perform Kernel Execution ---------------------------------------
			if (!TestTrue(TEXT("Invalid Vector Sizes!"), vectorA.size() == vectorB.size()))
				return;

			const size_t dataSize = numValues * sizeof(float);
			OpenCL::Buffer bufferA(mpDefaultDevice, context, vectorA.data(), dataSize, OpenCL::AccessType::READ_ONLY, OpenCL::MemoryStrategy::COPY_ONCE);
			OpenCL::Buffer bufferB(mpDefaultDevice, context, vectorB.data(), dataSize, OpenCL::AccessType::READ_ONLY, OpenCL::MemoryStrategy::COPY_ONCE);
			OpenCL::Buffer outputBuffer(mpDefaultDevice, context, nullptr, dataSize, OpenCL::AccessType::WRITE_ONLY, OpenCL::MemoryStrategy::STREAM);

			kernel.SetArgument<OpenCL::Buffer>(0, bufferA);
			kernel.SetArgument<OpenCL::Buffer>(1, bufferB);
			kernel.SetArgument<OpenCL::Buffer>(2, outputBuffer);

			if (!TestTrue(TEXT("Couldn't Set Kernel Arguments!"), kernel.IsValid()))
				return;

			queue.EnqueueRange(kernel, 1, &numValues);

			if (!TestTrue(TEXT("Couldn't Enqueue the Kernel!"), queue.IsValid()))
				return;

			outputBuffer.Fetch(queue, output.data(), numValues * sizeof(float));

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
