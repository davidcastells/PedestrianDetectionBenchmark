#include "OpenCLUtils.h"

#include <string>
#include <unistd.h> // readlink, chdir
#include "scoped_ptrs.h"



// This is the minimum alignment requirement to ensure DMA can be used.
const unsigned AOCL_ALIGNMENT = 64;

#ifdef _WIN32 // Windows
void *alignedMalloc(size_t size) {
  return _aligned_malloc (size, AOCL_ALIGNMENT);
}

void alignedFree(void * ptr) {
  _aligned_free(ptr);
}
#else          // Linux
void *alignedMalloc(size_t size) {
  void *result = NULL;
  posix_memalign (&result, AOCL_ALIGNMENT, size);
  return result;
}

void alignedFree(void * ptr) {
  free (ptr);
}
#endif


OpenCLUtils::OpenCLUtils()
{
}


OpenCLUtils::~OpenCLUtils()
{
}

std::string format(const char *fmt, ...)
{
    std::string result = "";

    char buffer[256];
    va_list args;
    va_start (args, fmt);
    vsprintf (buffer,fmt, args);
    
    result.append(buffer);
    
    va_end (args);
  
    return result;
}


cl_platform_id selectPlatform(cl_uint selected_platform_index)
{
	using namespace std;

	cl_uint num_of_platforms = 0;
	// get total number of available platforms:
	cl_int err = clGetPlatformIDs(0, 0, &num_of_platforms);
	SAMPLE_CHECK_ERRORS(err);

        printf("Num platforms: %d\n", num_of_platforms);
        fflush(stdout);
        
        // use vector for automatic memory management
	vector<cl_platform_id> platforms(num_of_platforms);
	// get IDs for all platforms:
	err = clGetPlatformIDs(num_of_platforms, &platforms[0], 0);
	SAMPLE_CHECK_ERRORS(err);

	
	bool by_index = true;

	
	printf(" Platforms (%d):", num_of_platforms);
        fflush(stdout);

	// TODO In case of empty platform name select the default platform or 0th platform?

	for (cl_uint i = 0; i < num_of_platforms; ++i)
	{
		// Get the length for the i-th platform name
		size_t platform_name_length = 0;
		err = clGetPlatformInfo(
			platforms[i],
			CL_PLATFORM_NAME,
			0,
			0,
			&platform_name_length
		);
		SAMPLE_CHECK_ERRORS(err);

		// Get the name itself for the i-th platform
		// use vector for automatic memory management
		char platform_name[platform_name_length];
		err = clGetPlatformInfo(
			platforms[i],
			CL_PLATFORM_NAME,
			platform_name_length,
			platform_name,
			0
		);
		SAMPLE_CHECK_ERRORS(err);

		printf("    [%d] %s", i,  platform_name);

		// decide if this i-th platform is what we are looking for
		// we select the first one matched skipping the next one if any
		//
		if (selected_platform_index == i || // we already selected the platform by index
                    //string(&platform_name[0]).find(required_platform_subname) != string::npos &&
                    selected_platform_index == num_of_platforms // haven't selected yet
                    )
		{
			printf(" [Selected]");
			selected_platform_index = i;
			// do not stop here, just want to see all available platforms
		}

		// TODO Something when more than one platform matches a given subname

		printf("\n");
                fflush(stdout);
	}

	if (by_index && selected_platform_index >= num_of_platforms)
	{
		throw Error(
			"Given index of platform (" + to_string(selected_platform_index) + ") "
			"is out of range of available platforms"
		);
	}

	/*if (!by_index && selected_platform_index >= num_of_platforms)
	{
		throw Error(
			"There is no found platform with name containing \"" +
			required_platform_subname + "\" as a substring\n"
		);
	}*/

	return platforms[selected_platform_index];
}

string opencl_error_to_str(cl_int error)
{
#define CASE_CL_CONSTANT(NAME) case NAME: return #NAME;

	// Suppose that no combinations are possible.
	// TODO: Test whether all error codes are listed here
	switch (error)
	{
		CASE_CL_CONSTANT(CL_SUCCESS)
			CASE_CL_CONSTANT(CL_DEVICE_NOT_FOUND)
			CASE_CL_CONSTANT(CL_DEVICE_NOT_AVAILABLE)
			CASE_CL_CONSTANT(CL_COMPILER_NOT_AVAILABLE)
			CASE_CL_CONSTANT(CL_MEM_OBJECT_ALLOCATION_FAILURE)
			CASE_CL_CONSTANT(CL_OUT_OF_RESOURCES)
			CASE_CL_CONSTANT(CL_OUT_OF_HOST_MEMORY)
			CASE_CL_CONSTANT(CL_PROFILING_INFO_NOT_AVAILABLE)
			CASE_CL_CONSTANT(CL_MEM_COPY_OVERLAP)
			CASE_CL_CONSTANT(CL_IMAGE_FORMAT_MISMATCH)
			CASE_CL_CONSTANT(CL_IMAGE_FORMAT_NOT_SUPPORTED)
			CASE_CL_CONSTANT(CL_BUILD_PROGRAM_FAILURE)
			CASE_CL_CONSTANT(CL_MAP_FAILURE)
			CASE_CL_CONSTANT(CL_MISALIGNED_SUB_BUFFER_OFFSET)
			CASE_CL_CONSTANT(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST)
			CASE_CL_CONSTANT(CL_INVALID_VALUE)
			CASE_CL_CONSTANT(CL_INVALID_DEVICE_TYPE)
			CASE_CL_CONSTANT(CL_INVALID_PLATFORM)
			CASE_CL_CONSTANT(CL_INVALID_DEVICE)
			CASE_CL_CONSTANT(CL_INVALID_CONTEXT)
			CASE_CL_CONSTANT(CL_INVALID_QUEUE_PROPERTIES)
			CASE_CL_CONSTANT(CL_INVALID_COMMAND_QUEUE)
			CASE_CL_CONSTANT(CL_INVALID_HOST_PTR)
			CASE_CL_CONSTANT(CL_INVALID_MEM_OBJECT)
			CASE_CL_CONSTANT(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR)
			CASE_CL_CONSTANT(CL_INVALID_IMAGE_SIZE)
			CASE_CL_CONSTANT(CL_INVALID_SAMPLER)
			CASE_CL_CONSTANT(CL_INVALID_BINARY)
			CASE_CL_CONSTANT(CL_INVALID_BUILD_OPTIONS)
			CASE_CL_CONSTANT(CL_INVALID_PROGRAM)
			CASE_CL_CONSTANT(CL_INVALID_PROGRAM_EXECUTABLE)
			CASE_CL_CONSTANT(CL_INVALID_KERNEL_NAME)
			CASE_CL_CONSTANT(CL_INVALID_KERNEL_DEFINITION)
			CASE_CL_CONSTANT(CL_INVALID_KERNEL)
			CASE_CL_CONSTANT(CL_INVALID_ARG_INDEX)
			CASE_CL_CONSTANT(CL_INVALID_ARG_VALUE)
			CASE_CL_CONSTANT(CL_INVALID_ARG_SIZE)
			CASE_CL_CONSTANT(CL_INVALID_KERNEL_ARGS)
			CASE_CL_CONSTANT(CL_INVALID_WORK_DIMENSION)
			CASE_CL_CONSTANT(CL_INVALID_WORK_GROUP_SIZE)
			CASE_CL_CONSTANT(CL_INVALID_WORK_ITEM_SIZE)
			CASE_CL_CONSTANT(CL_INVALID_GLOBAL_OFFSET)
			CASE_CL_CONSTANT(CL_INVALID_EVENT_WAIT_LIST)
			CASE_CL_CONSTANT(CL_INVALID_EVENT)
			CASE_CL_CONSTANT(CL_INVALID_OPERATION)
			CASE_CL_CONSTANT(CL_INVALID_GL_OBJECT)
			CASE_CL_CONSTANT(CL_INVALID_BUFFER_SIZE)
			CASE_CL_CONSTANT(CL_INVALID_MIP_LEVEL)
			CASE_CL_CONSTANT(CL_INVALID_GLOBAL_WORK_SIZE)
//			CASE_CL_CONSTANT(CL_INVALID_PROPERTY)
//			CASE_CL_CONSTANT(CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR)
	default:
		return "UNKNOWN ERROR CODE " + error;
	}

#undef CASE_CL_CONSTANT
}



cl_device_id selectDevice(cl_platform_id platform, cl_uint selected_device_index)
{
	using namespace std;

	// List devices of a given type only
	cl_device_type device_type = CL_DEVICE_TYPE_ALL; //  = parseDeviceType(device_type_name);

	cl_uint num_of_devices = 0;
	cl_int err = clGetDeviceIDs(
		platform,
		device_type,
		0,
		0,
		&num_of_devices
	);

	SAMPLE_CHECK_ERRORS(err);

	vector<cl_device_id> devices(num_of_devices);

	cl_device_id selectedDeviceId = 0;

	err = clGetDeviceIDs(
		platform,
		device_type,
		num_of_devices,
		&devices[0],
		0
	);
	SAMPLE_CHECK_ERRORS(err);

	printf("Devices (%d):\n",  num_of_devices);
	
	for (cl_uint i = 0; i < num_of_devices; ++i)
	{
		// Get the length for the i-th device name
		size_t device_name_length = 0;
		err = clGetDeviceInfo(
			devices[i],
			CL_DEVICE_NAME,
			0,
			0,
			&device_name_length
		);
		SAMPLE_CHECK_ERRORS(err);

		// Get the name itself for the i-th device
		// use vector for automatic memory management
		char device_name[device_name_length];
		err = clGetDeviceInfo(
			devices[i],
			CL_DEVICE_NAME,
			device_name_length,
			device_name,
			0
		);
		SAMPLE_CHECK_ERRORS(err);

		printf("    [%d] %s", i, device_name);
		
		if (i == selected_device_index)
		{
			printf(" [Selected]");
			selectedDeviceId = devices[i];
		}

		printf("\n");
                fflush(stdout);
	}

	return selectedDeviceId;
}

void pfn_notify(const char *errinfo, const void *private_info, size_t cb, void *user_data)
{
	fprintf(stderr, "[OCLFPGAJNI] OpenCL Error (via pfn_notify): %s\n", errinfo);
        fprintf(stderr, "[OCLFPGAJNI] private info: %p\n", private_info);
        fprintf(stderr, "[OCLFPGAJNI] user data: %p\n", user_data);
        fflush(stderr);
        
        //system("pwd");
}


string getPlatformName(cl_platform_id platform_id)
{
    // Get the length for the i-th platform name
    size_t platform_name_length = 0;
    cl_int      err = clGetPlatformInfo(
            platform_id,
            CL_PLATFORM_NAME,
            0,
            0,
            &platform_name_length
    );
    SAMPLE_CHECK_ERRORS(err);

    // Get the name itself for the i-th platform
    // use vector for automatic memory management
    vector<char> platform_name(platform_name_length);
    err = clGetPlatformInfo(
            platform_id,
            CL_PLATFORM_NAME,
            platform_name_length,
            &platform_name[0],
            0
    );
    SAMPLE_CHECK_ERRORS(err);

    string str =  &platform_name[0];
                
    return str;
}

string getDeviceName(cl_device_id device_id)
{
		size_t device_name_length = 0;
		cl_int err = clGetDeviceInfo(
			device_id,
			CL_DEVICE_NAME,
			0,
			0,
			&device_name_length
		);
		SAMPLE_CHECK_ERRORS(err);

		// Get the name itself for the i-th device
		// use vector for automatic memory management
		vector<char> device_name(device_name_length);
		err = clGetDeviceInfo(
			device_id,
			CL_DEVICE_NAME,
			device_name_length,
			&device_name[0],
			0
		);
		SAMPLE_CHECK_ERRORS(err);

		string str =  &device_name[0];
                
                return str;
    
}





cl_context createContext(cl_platform_id platform, cl_device_id device/*const cl_context_properties* additional_context_props*/)
{
	using namespace std;

	if (!platform)
	{
		throw Error("Platform is not selected");
	}

	if (!device)
	{
		throw Error("Device is not selected");
	}

	size_t number_of_additional_props = 0;
	/*if (additional_context_props)
	{
		// count all additional props including terminating 0
		while (additional_context_props[number_of_additional_props++]);
		number_of_additional_props--;   // now exclude terminating 0
	}*/

	// allocate enough space for platform and all additional props if any
	std::vector<cl_context_properties> context_props(
		2 + // for CL_CONTEXT_PLATFORM and platform itself
		number_of_additional_props +
		1   // for terminating zero
	);

	context_props[0] = CL_CONTEXT_PLATFORM;
	context_props[1] = cl_context_properties(platform);
	/*
	std::copy(
		additional_context_props,
		additional_context_props + number_of_additional_props,
		context_props.begin() + 2   // +2 -- skipping already initialized platform entries
	);*/

	context_props.back() = 0;

	cl_int err = 0;
	cl_context context = clCreateContext(&context_props[0], 1, &device, pfn_notify, 0, &err);
	SAMPLE_CHECK_ERRORS(err);

	return context;
}

cl_command_queue createQueue(cl_device_id device, cl_context context, cl_command_queue_properties queue_properties)
{
	using namespace std;

	if (!device)
	{
		throw Error("Device is not selected");
	}

	cl_int err = 0;
	cl_command_queue queue = clCreateCommandQueue(context, device, queue_properties, &err);
	SAMPLE_CHECK_ERRORS(err);

	return queue;
}


bool fileExists(const char *file_name) {
#ifdef _WIN32 // Windows
  DWORD attrib = GetFileAttributesA(file_name);
  return (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY));
#else         // Linux
  return access(file_name, R_OK) != -1;
#endif
}

using namespace aocl_utils;

char* readCodeFromFile(const char* srcFile)
{
	FILE *fIn = fopen(srcFile, "r");

        if (fIn == NULL)
        {
            cerr << "Failed to load file " << srcFile << endl;
            exit(-1);
        }
	// Error check the fIn here
	// get the size
	fseek(fIn, 0L, SEEK_END);
	size_t sz = ftell(fIn);
	rewind(fIn);
	int lenFileText = sizeof(char)*sz + 1;
	char *file = (char*)malloc(lenFileText);

	memset(file, 0, lenFileText);
	fread(file, sizeof(char), sz, fIn);
	
	fclose(fIn);
	return file;
}

// Loads a file in binary form.
unsigned char *loadBinaryFile(const char *file_name, size_t *size) {
  // Open the File
  FILE* fp;
#ifdef _WIN32
  if(fopen_s(&fp, file_name, "rb") != 0) {
    return NULL;
  }
#else
  fp = fopen(file_name, "rb");
  if(fp == 0) {
    return NULL;
  }
#endif

  // Get the size of the file
  fseek(fp, 0, SEEK_END);
  *size = ftell(fp);

  // Allocate space for the binary
  unsigned char *binary = new unsigned char[*size];

  printf("Binary File Size: %d\n", (int) *size);
  
  // Go back to the file start
  //fseek(fp, 0, SEEK_SET);
  rewind(fp);

  int total = *size;
  int offset = 0;
  int nread = 0;
  
  // Read the file into the binary
  /*do
  {
      nread = fread((void*)&binary[offset], 1, 0x400 * 64, fp);
      offset += nread;
      total -= nread;
      
      printf("loading [ %f %%] %d\n", (offset*100.0/(*size)), nread);
      
      if (nread <= 0 && total > 0)
      {
          delete[] binary;
          fclose(fp);
          printf("### ERROR ### could not load binary");
          return NULL;
      }
  } while (total > 0);*/
  
  if (fread((void*)binary, 1, *size, fp) < *size)
    {
        delete[] binary;
        fclose(fp);
        printf("### ERROR ### could not load binary");
        return NULL;
    }
  
      printf("Binary Read: [OK]\n");

  return binary;
}

// Create a program for all devices associated with the context.
cl_program createProgramFromBinary(cl_context context, const char *binary_file_name, const cl_device_id *devices, unsigned num_devices) 
{
    printf("Loading %s...\n", binary_file_name);
    printf("For %d devices...\n", num_devices);
    
  // Early exit for potentially the most common way to fail: AOCX does not exist.
  if(!fileExists(binary_file_name)) 
  {
    printf("AOCX file '%s' does not exist.\n", binary_file_name);
    throw Error("Failed to load binary file");
  }

  // Load the binary.
  size_t binary_size;
  unsigned char* binary = loadBinaryFile(binary_file_name, &binary_size);
  
  if (binary == NULL) 
  {
      throw Error("Failed to load binary file");
  }

  size_t binary_lengths[num_devices];
  unsigned char * binaries[num_devices];
  
  for (int i = 0; i < num_devices; i++) 
  {
    binary_lengths[i] = binary_size;
    binaries[i] = binary;
  }

  cl_int status;
  scoped_array<cl_int> binary_status(num_devices);

  cl_program program = clCreateProgramWithBinary(context, num_devices, devices, binary_lengths,
      (const unsigned char **) binaries, binary_status, &status);

  printf("create program finished\n");
  fflush(stdout);
  //checkError(status, "Failed to create program with binary");
  SAMPLE_CHECK_ERRORS(status);
//  for(unsigned i = 0; i < num_devices; ++i) {
//    throw Error("Failed to load binary for device");
//  }

  return program;
}

static const char *const VERSION_STR = "161";

std::string getBoardBinaryFile(const char *prefix, cl_device_id device)
{
  // First check if <prefix>.aocx exists. Use it if it does.
  std::string file_name = std::string(prefix) + ".aocx";
  if(fileExists(file_name.c_str())) {
    return file_name;
  }

  // Now get the name of the board. For Intel(R) FPGA SDK for OpenCL(TM) boards,
  // the name of the device is presented as:
  //  <board name> : ...
  std::string device_name = getDeviceName(device);

  // Now search for the " :" in the device name.
  size_t end = device_name.find(" :");
  if(end != std::string::npos) {
    std::string board_name(device_name, 0, end);

    // Look for a AOCX with the name <prefix>_<board_name>_<version>.aocx.
    file_name = std::string(prefix) + "_" + board_name + "_" + VERSION_STR + ".aocx";
    if(fileExists(file_name.c_str())) {
      return file_name;
    }
  }

  // At this point just use <prefix>.aocx. This file doesn't exist
  // and this should trigger an error later.
  return std::string(prefix) + ".aocx";
}

bool setCwd(const char* path)
{
  // Change the current directory.
#ifdef _WIN32 // Windows
  SetCurrentDirectoryA(path);
#else         // Linux
  int rc;
  rc = chdir(path);
#endif

  return true;
}


// Sets the current working directory to be the same as the directory
// containing the running executable.
bool setCwdToExeDir() 
{
#ifdef _WIN32 // Windows
  HMODULE hMod = GetModuleHandle(NULL);
  char path[MAX_PATH];
  GetModuleFileNameA(hMod, path, MAX_PATH);

#else         // Linux
  // Get path of executable.
  char path[300];
  ssize_t n = readlink("/proc/self/exe", path, sizeof(path)/sizeof(path[0]) - 1);
  if(n == -1) {
    return false;
  }
  path[n] = 0;
#endif

  // Find the last '\' or '/' and terminate the path there; it is now
  // the directory containing the executable.
  size_t i;
  for(i = strlen(path) - 1; i > 0 && path[i] != '/' && path[i] != '\\'; --i);
  path[i] = '\0';

  // Change the current directory.
#ifdef _WIN32 // Windows
  SetCurrentDirectoryA(path);
#else         // Linux
  int rc;
  rc = chdir(path);
#endif

  return true;
}
