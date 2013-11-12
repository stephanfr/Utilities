/*
Copyright (c) 2013 Stephan Friedl

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

Except as contained in this notice, the name(s) of the above copyright holders
shall not be used in advertising or otherwise to promote the sale, use or other
dealings in this Software without prior written authorization.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/



#ifndef PLUGINLOADER_H_
#define PLUGINLOADER_H_

#include <memory>
#include <dlfcn.h>

#include "Result.h"



namespace SEFUtility
{



	template<typename TPlugin> class PluginLoader
	{
	public:

		enum class ErrorCodes { SUCCESS = 0,
								COULD_NOT_LOAD_SHARED_OBJECT,
								COULD_NOT_BIND_FACTORY_FUNCTION,
								EXCEPTION_CAUGHT_FROM_FACTORY_FUNCTION,
								EXCEPTION_CAUGHT };


		typedef ResultWithReturnPtr<ErrorCodes, TPlugin>		Result;

		static Result		LoadPlugin( std::string 	soName,
										std::string		factoryName )
		{
			try
			{
				//	Load the shared object.  If we get NULL back, then we could not load it, so return a failed result.

				void*		sharedLibrary = dlopen( soName.c_str(), RTLD_LAZY );

				if( sharedLibrary == NULL )
				{
					return( Result::Failure( ErrorCodes::COULD_NOT_LOAD_SHARED_OBJECT, std::string( "Unable to load shared object: " + soName + "  Reason: " + dlerror() ) ) );
				}

				//	Load the factory function

				void* (*factoryFunction)() = (void*(*)())dlsym( sharedLibrary, factoryName.c_str() );

				if( factoryFunction == NULL )
				{
					return( Result::Failure( ErrorCodes::COULD_NOT_BIND_FACTORY_FUNCTION, std::string( "Unable to bind factory function: " + factoryName + " in shared object: " + soName ) ) );
				}

				//	Invoke the factory function

				void*		pluginObjectAsVoidPtr;

				try
				{
					pluginObjectAsVoidPtr = factoryFunction();
				}
				catch(...)
				{
					return( Result::Failure( ErrorCodes::EXCEPTION_CAUGHT_FROM_FACTORY_FUNCTION, std::string( "Exception caught while invoking factory function: " + factoryName + " in shared object: " + soName ) ) );
				}

				//	Return a success result with a pointer to the object returned from the factory function

				std::unique_ptr<TPlugin>	pluginObject( (TPlugin*)( pluginObjectAsVoidPtr ));

				return( Result( pluginObject ));
			}
			catch(...)
			{
				return( Result::Failure( ErrorCodes::EXCEPTION_CAUGHT, std::string( "Exception caught somewhere during load of plugin from shared object: " + soName ) ) );
			}
		}
	};

} /* namespace Utility */
#endif /* PLUGINLOADER_H_ */
