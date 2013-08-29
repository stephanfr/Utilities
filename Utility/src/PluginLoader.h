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


namespace SEFUtility
{



	template<typename TPlugin> class PluginLoader
	{
	public:

		enum class ErrorCodes { SUCCESS,
								COULD_NOT_LOAD_SHARED_OBJECT,
								COULD_NOT_BIND_FACTORY_FUNCTION,
								EXCEPTION_CAUGHT_FROM_FACTORY_FUNCTION,
								EXCEPTION_CAUGHT };


		typedef ResultWithReturnPtr<ErrorCodes, TPlugin >		Result;

		static Result		LoadPlugin( std::string 	soName,
										std::string		factoryName )
		{
			try
			{
				//	Load the shared object.  If we get NULL back, then we could not load it, so return a failed result.

				void*		sharedLibrary = dlopen( soName.c_str(), RTLD_LAZY );

				if( sharedLibrary == NULL )
				{
					return( Result::Failure( ErrorCodes::COULD_NOT_LOAD_SHARED_OBJECT, std::string( "Unable to load shared object: " + soName ) ) );
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

				return( Result::Success( pluginObject ));
			}
			catch(...)
			{
				return( Result::Failure( ErrorCodes::EXCEPTION_CAUGHT, std::string( "Exception caught somewhere during load of plugin from shared object: " + soName ) ) );
			}
		}
	};

} /* namespace Utility */
#endif /* PLUGINLOADER_H_ */
