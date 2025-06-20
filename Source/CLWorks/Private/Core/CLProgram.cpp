#include "Core/CLProgram.h"

#include "CLWorksLog.h"

#include <sstream>
#include <fstream>

namespace OpenCL
{
	Program::Program()
		: mpProgram(nullptr),
		mpContext(),
		mpDevice()
	{
	}

	Program::Program(const std::shared_ptr<Context>& context,
					 const std::shared_ptr<Device>& device)
		: mpProgram(nullptr),
		mpContext(context),
		mpDevice(device)
	{
	}

	Program::~Program()
	{
		if (mpProgram)
		{
			clReleaseProgram(mpProgram);
			mpProgram = nullptr;
		}
	}

	bool Program::ReadFromFile(const std::filesystem::path& file,
							   std::string* errMsg)
	{
		mpProgram = nullptr;

		std::unordered_set<std::string> includedFiles;
		const std::string program_string = ReadProgram(file, includedFiles, errMsg);

		if (program_string.empty())
		{
			if (errMsg)
				*errMsg = "Program File is Empty or Could Not be Read.";
			return false;
		}

		return SetupProgramFromString(program_string, errMsg);
	}

	bool Program::ReadFromString(const std::string& program,
								 std::string* errMsg)
	{
		mpProgram = nullptr;

		return SetupProgramFromString(program, errMsg);
	}

	bool Program::SetupProgramFromString(const std::string& programString,
										 std::string* errMsg)
	{
		const std::shared_ptr<Context> context_ptr = mpContext.lock();
		if (!context_ptr)
		{
			UE_LOG(LogCLWorks, Warning, TEXT("Invalid Program Context!"));
			return false;
		}

		const std::shared_ptr<Device> device_ptr = mpDevice.lock();
		if (!device_ptr)
		{
			UE_LOG(LogCLWorks, Warning, TEXT("Invalid Program Device!"));
			return false;
		}

		cl_program program;
		char* program_buffer, * program_log;
		size_t program_size, log_size;

		int32_t err = 0;

		program_size = programString.size();
		program_buffer = (char*)malloc(program_size + 1);
		program_buffer[program_size] = '\0';
		std::memcpy(program_buffer, programString.c_str(), program_size);

		program = clCreateProgramWithSource(context_ptr->Get(),
											1,
											(const char**)&program_buffer, 
											&program_size,
											&err);

		if (err < 0)
		{
			if (errMsg)
				*errMsg = "Couldn't Create the Program!";
			else
				UE_LOG(LogCLWorks, Error, TEXT("Couldn't Create the Program!"));
			return false;
		}
		free(program_buffer);

		err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
		if (err < 0) 
		{
			/* Find size of log and print to std output */
			clGetProgramBuildInfo(program, 
								  device_ptr->Get(), 
								  CL_PROGRAM_BUILD_LOG,
								  0, 
								  NULL, 
								  &log_size);

			program_log = (char*)malloc(log_size + 1);
			program_log[log_size] = '\0';

			clGetProgramBuildInfo(program, 
								  device_ptr->Get(), 
								  CL_PROGRAM_BUILD_LOG,
								  log_size + 1, 
								  program_log, 
								  NULL);

			if (errMsg)
			{
				*errMsg = "Program Error: " + std::string(program_log);
			}
			else
			{
				UE_LOG(LogCLWorks, Error, TEXT("Program Error: %s"), *FString(program_log));
			}

			free(program_log);
			return false;
		}

		mpProgram = program;
		return true;
	}

	std::string Program::ReadProgram(const std::filesystem::path& file,
									 std::unordered_set<std::string>& includedFiles,
									 std::string* errMsg)
	{
		if (!std::filesystem::exists(file))
		{
			if (errMsg)
				*errMsg = "File Couldn't Be Found At: " + file.string();
			else
				UE_LOG(LogCLWorks, Error, TEXT("File Couldn't Be Found At: %s"), *FString(file.c_str()));
			return "";
		}

		/* Read program file and place content into buffer */
		std::ifstream program_file;
		program_file.open(file);

		if (!program_file.is_open()) 
		{
			if (errMsg)
				*errMsg = "Could Not Open File: " + file.string();
			else
				UE_LOG(LogCLWorks, Error, TEXT("Could Not Open File: %s"), *FString(file.c_str()));
			return "";
		}

		std::stringstream buffer;
		buffer << program_file.rdbuf();
		std::string program_string = buffer.str();
		if (program_string.find("#include") != std::string::npos)
		{
			std::istringstream iss(program_string);
			std::string line;
			while (std::getline(iss, line))
			{
				if (line.find("#include") != std::string::npos)
				{
					std::string includeFile = line.substr(line.find_first_of("\"") + 1, line.find_last_of("\"") - line.find_first_of("\"") - 1);
					if (includedFiles.find(includeFile) == includedFiles.end())
					{
						includedFiles.insert(includeFile);
						std::filesystem::path includeFilePath = file.parent_path() / includeFile;
						std::string includeFileContent = ReadProgram(includeFilePath, includedFiles, errMsg);
						program_string.replace(program_string.find(line), line.size(), includeFileContent);
					}
				}
			}
		}
		return program_string;
	}
}