/*
 * PluginLoader.h
 *
 *  Created on: Aug 14, 2013
 *      Author: steve
 */

#ifndef PLUGINLOADER_H_
#define PLUGINLOADER_H_

#include <memory>
#include <dlfcn.h>

#include <Result.h>


namespace Utility
{



	template<typename TPlugin> class PluginLoader
	{
	public:

		enum class ErrorCodes { SUCCESS, COULD_NOT_LOAD_SHARED_OBJECT };

		typedef ResultWithReturnPtr<ErrorCodes, TPlugin >		Result;

		static Result		LoadPlugin( std::string 	soName,
										std::string		factoryName )
		{
			//	Load the shared object.  If we get NULL back, then we could not load it, so return a failed result.

			void*		sharedLibrary = dlopen( soName.c_str(), RTLD_LAZY );

			if( sharedLibrary == NULL )
			{
				return( Result::Failure( ErrorCodes::COULD_NOT_LOAD_SHARED_OBJECT, std::string( "Unable to load shared object: " + soName ) ) );
			}

			//	Load the factory function

			void* (*factoryFunction)() = (void*(*)())dlsym( sharedLibrary, factoryName.c_str() );

			void*		pluginObjectAsVoidPtr = factoryFunction();

			std::unique_ptr<TPlugin>	pluginObject( (TPlugin*)( pluginObjectAsVoidPtr ));

			return( Result::Success( pluginObject ));
		}
	};

} /* namespace Utility */
#endif /* PLUGINLOADER_H_ */
