#ifndef CEDEFS_H__INCLUDED
#define CEDEFS_H__INCLUDED

#if defined(CE_DLL_BUILD) && defined(CE_FROM_DLL)
#error Define CE_DLL_BUILD or CE_FROM_DLL, but not both
#endif

#ifndef CRYSEDIT_CLASS_DECL
	#if defined(CE_DLL_BUILD)
		#define CRYSEDIT_CLASS_DECL		__declspec(dllexport)
	#elif defined(CE_FROM_DLL)
		#define CRYSEDIT_CLASS_DECL		__declspec(dllimport)
	#else
		#define CRYSEDIT_CLASS_DECL
	#endif
#endif

#ifndef CE_INLINE
	#if defined(CE_DLL_BUILD) || defined(CE_FROM_DLL)
		#define CE_INLINE
	#else
		#define CE_INLINE inline
	#endif
#endif

#endif
