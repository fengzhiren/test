#include "cstring.h"
#include<string>
int main()
{
	std::string s;
	if(s=="")
	{ 
		std::cout << "empty" << std::endl;
	}
	const std::string s1 = "1";
	s = s1 + "1";
	if (s == "")
	{
		std::cout << "Yes" << std::endl;
	}
	const CString oStr1="1";
	CString ostr2 = oStr1 + "1";
	return 0;
}