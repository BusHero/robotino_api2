#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>

#include <ctime>

#define FILLBYTE 0xFF

void printHelp()
{
	std::cout << "bin2c binfile cfile" << std::endl;
	std::cout << "Version 1.0.0" << std::endl;
}

int main( int argc, char** argv )
{
	if( argc != 3
		|| !strcmp( argv[1], "-h" )
		|| !strcmp( argv[1], "--help" )
		|| !strcmp( argv[1], "/?" )
		)
	{
		printHelp();
		return 0;
	}

	std::string binfile = argv[1];
	const std::string cfile = argv[2];

	char* binfileData = 0;
	unsigned int binfileDataSize = 0;

	std::ifstream is( binfile.c_str(), std::ios::in | std::ios::binary | std::ios::ate );
  if( !is.is_open() )
  {
    std::cout << "Error reading " << binfile << std::endl;
		return 1;
  }
  else
  {
    binfileDataSize = is.tellg();

		bool needsFill = false;

		if( binfileDataSize % 2 != 0 )
		{
			binfileDataSize += 1;
		}

		binfileData = new char[ binfileDataSize ];
		memset( binfileData, 0xFF, binfileDataSize );

    is.seekg( 0, std::ios::beg );
    is.read( binfileData, binfileDataSize );
    is.close();
  }

	std::ofstream of;
	of.open( cfile.c_str() );
	if( !of.is_open() )
	{
		std::cout << "Error opening " << cfile << std::endl;
		return 1;
	}

	time_t tnow = time( NULL );
	struct tm* tmlocal = localtime( &tnow );

	of << "#include <absacc.h>" << std::endl;
	of << std::endl;

	of << "const char fpga_config_version[] = \""
		<< ( 1900 + tmlocal->tm_year )
		<< std::setw( 2 ) << std::setfill( '0' ) << ( tmlocal->tm_mon + 1 )
		<< std::setw( 2 ) << std::setfill( '0' ) << tmlocal->tm_mday
		<< "\";";
	of << std::endl;

	std::replace( binfile.begin(), binfile.end(), '\\', '/' );

	of << "const char fpga_source_file[] = \""
		<< binfile
		<< "\";";
	of << std::endl;

	of << "const unsigned int fpga_config_size = " << binfileDataSize/2 << ";" << std::endl;
	of << std::endl;

	//of << "/**" << std::endl;
	//of << "Place the FPGA configuration in flash memory" << std::endl;
	//of << "on-chip flash memory address range is 0x0000 0000 to 0x0007 FFFF" << std::endl;
	//of << "we use the upper 200kB for the FPGA configuration 0x7FFFF - 256*1024 + 1 = 0x40000" << std::endl;
	//of << "*/" << std::endl;
	//of << std::endl;
	
	//of << "const unsigned char fpga_config[] __at( 0x40000 ) = {" << std::endl;
	of << "const unsigned short fpga_config[] = {" << std::endl;

	unsigned char* data = reinterpret_cast<unsigned char*>( binfileData );
	int col = 0;
	for( unsigned int i=0; i<binfileDataSize/2; ++i )
	{
		of << "0x" << std::hex << std::setfill( '0' ) << std::setw( 2 ) << static_cast<unsigned int>(*data)
			<< std::hex << std::setfill( '0' ) << std::setw( 2 ) << static_cast<unsigned int>(*(data+1));
		if( i+1 < binfileDataSize/2 )
		{
			of << " , ";
		}
		else
		{
			of << std::endl << "};" << std::endl;
		}

		data += 2;
		++col;
		if( col == 16 )
		{
			of << std::endl;
			col = 0;
		}
	}

	return 0;
}