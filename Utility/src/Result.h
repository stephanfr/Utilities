/*
 * Result.h
 *
 *  Created on: Jul 1, 2013
 *      Author: steve
 */

#ifndef RESULT_H_
#define RESULT_H_


#include <memory>
#include <string>


namespace Utility
{

	enum class BaseResultCodes { SUCCESS = 0, FAILURE };


	template<typename TErrorCodeEnum> class Result
	{

	protected :

		Result( BaseResultCodes			successOrFailure,
				TErrorCodeEnum			errorCode,
				const char*				message )
			: m_successOrFailure( successOrFailure ),
			  m_errorCode( errorCode ),
			  m_message( message )
		{}

		Result( BaseResultCodes			successOrFailure,
				TErrorCodeEnum			errorCode,
				const std::string		message )
			: m_successOrFailure( successOrFailure ),
			  m_errorCode( errorCode ),
			  m_message( message )
		{}

	public :

		virtual ~Result() {};

		static Result<TErrorCodeEnum>		Success()
		{
			return( Result( BaseResultCodes::SUCCESS, TErrorCodeEnum::SUCCESS, "Success" ));
		};

		static Result<TErrorCodeEnum>		Failure( TErrorCodeEnum		errorCode,
									 	 	 	 	 const char*		message )
		{
			return( Result( BaseResultCodes::FAILURE, errorCode, message ));
		}

		static Result<TErrorCodeEnum>		Failure( TErrorCodeEnum		errorCode,
													 const std::string	message )
		{
			return( Result( BaseResultCodes::FAILURE, errorCode, message ));
		}


		bool				Succeeded() const
		{
			return( m_successOrFailure == BaseResultCodes::SUCCESS );
		}

		bool				Failed() const
		{
			return( m_successOrFailure == BaseResultCodes::FAILURE );
		}


		TErrorCodeEnum		errorCode() const
		{
			return( m_errorCode );
		}

		const std::string	message() const
		{
			return( m_message );
		}

	private :

		BaseResultCodes		m_successOrFailure;

		TErrorCodeEnum		m_errorCode;

		std::string			m_message;
	};



	template <typename TErrorCodeEnum, typename TResultType> class ResultWithReturnValue : Result<TErrorCodeEnum>
	{
	private :

		ResultWithReturnValue( BaseResultCodes			successOrFailure,
							   TErrorCodeEnum			errorCode,
							   const char*				message,
							   TResultType				returnValue )
		: Result<TErrorCodeEnum>( successOrFailure, errorCode, message ),
		  m_returnValue( returnValue )
		{}

		ResultWithReturnValue( BaseResultCodes			successOrFailure,
							   TErrorCodeEnum			errorCode,
							   const std::string		message,
							   TResultType				returnValue )
		: Result<TErrorCodeEnum>( successOrFailure, errorCode, message ),
		  m_returnValue( returnValue )
		{}

		ResultWithReturnValue( BaseResultCodes			successOrFailure,
							   TErrorCodeEnum			errorCode,
							   const char*				message )
		: Result<TErrorCodeEnum>( successOrFailure, errorCode, message )
		{}

		ResultWithReturnValue( BaseResultCodes			successOrFailure,
							   TErrorCodeEnum			errorCode,
							   const std::string		message )
		: Result<TErrorCodeEnum>( successOrFailure, errorCode, message )
		{}

	public :

		virtual ~ResultWithReturnValue() {};

		static ResultWithReturnValue<TErrorCodeEnum,TResultType>		Success( TResultType	returnValue )
		{
			return( ResultWithReturnValue( BaseResultCodes::SUCCESS, TErrorCodeEnum::SUCCESS, "Success", returnValue ));
		};

		static ResultWithReturnValue<TErrorCodeEnum,TResultType>		Failure( TErrorCodeEnum		errorCode,
									 const char*		message )
		{
			return( ResultWithReturnValue( BaseResultCodes::FAILURE, errorCode, message ));
		}

		static ResultWithReturnValue<TErrorCodeEnum,TResultType>		Failure( TErrorCodeEnum		errorCode,
									 const std::string	message )
		{
			return( ResultWithReturnValue( BaseResultCodes::FAILURE, errorCode, message ));
		}


		TResultType&			ReturnValue()
		{
			return( m_returnValue );
		}

	private :

		TResultType			m_returnValue;
	};



	template <typename TErrorCodeEnum, typename TResultType> class ResultWithReturnPtr : Result<TErrorCodeEnum>
	{
	private :

		ResultWithReturnPtr( BaseResultCodes					successOrFailure,
							   TErrorCodeEnum					errorCode,
							   const char*						message,
							   std::unique_ptr<TResultType>&	returnValue )
		: Result<TErrorCodeEnum>( successOrFailure, errorCode, 	message ),
		  m_returnValue( std::move( returnValue ))
		{}

		ResultWithReturnPtr( BaseResultCodes					successOrFailure,
							   TErrorCodeEnum					errorCode,
							   const std::string				message,
							   std::unique_ptr<TResultType>&	returnValue )
		: Result<TErrorCodeEnum>( successOrFailure, errorCode, message ),
		  m_returnValue( std::move( returnValue ))
		{}

		ResultWithReturnPtr( BaseResultCodes			successOrFailure,
							   TErrorCodeEnum			errorCode,
							   const char*				message )
		: Result<TErrorCodeEnum>( successOrFailure, errorCode, message )
		{}

		ResultWithReturnPtr( BaseResultCodes			successOrFailure,
							   TErrorCodeEnum			errorCode,
							   const std::string		message )
		: Result<TErrorCodeEnum>( successOrFailure, errorCode, message )
		{}

	public :

		virtual ~ResultWithReturnPtr() {};

		static ResultWithReturnPtr<TErrorCodeEnum,TResultType>		Success( std::unique_ptr<TResultType>&	returnValue )
		{
			return( ResultWithReturnPtr<TErrorCodeEnum,TResultType>( BaseResultCodes::SUCCESS, TErrorCodeEnum::SUCCESS, "Success", returnValue ));
		};

		static ResultWithReturnPtr<TErrorCodeEnum,TResultType>		Failure( TErrorCodeEnum		errorCode,
									 	 	 	 	 	 	 	 	 	 	 	 	 	 	 const char*		message )
		{
			return( ResultWithReturnPtr<TErrorCodeEnum,TResultType>( BaseResultCodes::FAILURE, errorCode, message ));
		}

		static ResultWithReturnPtr<TErrorCodeEnum,TResultType>		Failure( TErrorCodeEnum		errorCode,
									 	 	 	 	 	 	 	 	 	 	 	 	 	 	 const std::string	message )
		{
			return( ResultWithReturnPtr<TErrorCodeEnum,TResultType>( BaseResultCodes::FAILURE, errorCode, message ));
		}


		std::unique_ptr<TResultType>&			ReturnPtr()
		{
			return( m_returnValue );
		}

	private :

		std::unique_ptr<TResultType>			m_returnValue;
	};


} /* namespace Utility */
#endif /* RESULT_H_ */