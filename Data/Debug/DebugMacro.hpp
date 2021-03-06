#ifndef INCLUDE_WILDCAT_DEBUG_DEBUG_MACRO_HPP
#define INCLUDE_WILDCAT_DEBUG_DEBUG_MACRO_HPP

/*
	#include <Debug/DebugMacro.hpp>

	Macro to output whatever is enclosed in _( ).

	For example, 

		_(std::cout<<"\nGarosoft: Project ETNA WIP.\n\n";)
	Will output
		std::cout<<"\nGarosoft: Project ETNA WIP.\n\n";
	to the console.
*/

	#ifndef DISABLE_DEBUG_MACRO
		#define _(x) std::cout << "DBG " << #x << std::endl; x
	#else
		#define _(x) x
	#endif
	
	/* Easy, you just put DBG wherever you want a kind of breakpoint. */
	#define DBG std::cout<<"DBG:"<<__FILE__<<":"<<__func__<<":"<<__LINE__<<"\n";

#endif
