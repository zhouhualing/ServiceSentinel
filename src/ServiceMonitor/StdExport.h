#ifndef STD_EXPORT_H
#define STD_EXPORT_H

#ifdef STD_EXPORTS
	#if defined WIN32
		#define STD_PUBLIC __declspec(dllexport)
	#else
		#define STD_PUBLIC
	#endif
#else
	#if defined WIN32
		#define STD_PUBLIC __declspec(dllimport)
	#else
		#define STD_PUBLIC
	#endif
#endif


#endif // STD_EXPORT_H

