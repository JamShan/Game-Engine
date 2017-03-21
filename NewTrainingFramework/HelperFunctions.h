#pragma once
#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>

#include <GLES2\gl2.h>

#include "Vertex.h"

#include "../Utilities/Math.h"
#include "../Utilities/utilities.h"

namespace helperfunctions
{
	// helper function for parsing
	template< typename T >
	T stringToNumber(const std::string s) 	//  string to number conversion
	{
		std::stringstream ss;
		ss << s;
		T nr;
		ss >> nr;
		return nr;
	}
	
}// namespace