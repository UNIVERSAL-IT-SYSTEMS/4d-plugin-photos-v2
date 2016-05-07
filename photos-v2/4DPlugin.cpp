/* --------------------------------------------------------------------------------
 #
 #	4DPlugin.cpp
 #	source generated by 4D Plugin Wizard
 #	Project : Photos v2
 #	author : miyako
 #	2016/05/06
 #
 # --------------------------------------------------------------------------------*/


#include "4DPluginAPI.h"
#include "4DPlugin.h"

static PhotosApplication *Photos = [SBApplication applicationWithBundleIdentifier:@"com.apple.Photos"];

method_id_t methodId;

void PluginMain(PA_long32 selector, PA_PluginParameters params)
{
	try
	{
		PA_long32 pProcNum = selector;
		sLONG_PTR *pResult = (sLONG_PTR *)params->fResult;
		PackagePtr pParams = (PackagePtr)params->fParameters;

		CommandDispatcher(pProcNum, pResult, pParams); 
	}
	catch(...)
	{

	}
}

void CommandDispatcher (PA_long32 pProcNum, sLONG_PTR *pResult, PackagePtr pParams)
{
	switch(pProcNum)
	{
// --- Photos

		case 1 :
			Photos_GET_SELECTION(pResult, pParams);
			break;

	}
}

// ------------------------------------ Photos ------------------------------------

void importImages()
{
	BOOL usingOriginals = YES;//NO returns larger images
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
	NSString *cachesFolder = [paths objectAtIndex:0];
	NSArray *photos = [Photos selection];
	NSUInteger count = [photos count];
	for(NSUInteger i = 0; i < count; ++i)
	{
		PA_YieldAbsolute();
		PhotosMediaItem *photo = (PhotosMediaItem *)[photos objectAtIndex:i];
		
		if((photo) && ([photo id]) && ([photo filename]) && ([photo date]))
		{
			PA_Variable params[6];
			params[0] = PA_CreateVariable(eVK_Picture);
			params[1] = PA_CreateVariable(eVK_Unistring);
			params[2] = PA_CreateVariable(eVK_Unistring);
			params[3] = PA_CreateVariable(eVK_Unistring);
			params[4] = PA_CreateVariable(eVK_Longint);
			params[5] = PA_CreateVariable(eVK_Longint);
			
			C_TEXT t1, t2, t3;
			t1.setUTF16String([photo id]);
			t2.setUTF16String([photo filename]);
			t3.setUTF16String([[photo date]description]);
			
			PA_Unistring v1 = PA_CreateUnistring((PA_Unichar *)t1.getUTF16StringPtr());
			PA_SetStringVariable(&params[1], &v1);
			PA_Unistring v2 = PA_CreateUnistring((PA_Unichar *)t2.getUTF16StringPtr());
			PA_SetStringVariable(&params[2], &v2);
			PA_Unistring v3 = PA_CreateUnistring((PA_Unichar *)t3.getUTF16StringPtr());
			PA_SetStringVariable(&params[3], &v3);
			
			PA_SetLongintVariable(&params[4], i+1);
			PA_SetLongintVariable(&params[5], count);
			
			//setup
			NSUUID *uuid = [[NSUUID alloc]init];
			NSString *folderName = [uuid UUIDString];
			NSURL *cachesFolderUrl = (NSURL *)CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)cachesFolder, kCFURLPOSIXPathStyle, true);
			
			//export
			NSURL *exportFolderUrl = [cachesFolderUrl URLByAppendingPathComponent:folderName isDirectory:true];
			[Photos export:[NSArray arrayWithObject:photo] to:exportFolderUrl usingOriginals:usingOriginals];
			NSURL *exportFileUrl;
			
			if(!usingOriginals)
			{
				exportFileUrl = [exportFolderUrl URLByAppendingPathComponent:[[photo.filename stringByDeletingPathExtension]stringByAppendingString:@".jpg"] isDirectory:false];
			}else{
				exportFileUrl = [exportFolderUrl URLByAppendingPathComponent:photo.filename isDirectory:false];
			}
			NSData *data = [[NSData alloc]initWithContentsOfURL:exportFileUrl];
			
			if(data)
			{
				PA_Picture picture = PA_CreatePicture((void*)[data bytes], [data length]);
				PA_SetPictureVariable(&params[0], picture);
				PA_Variable result = PA_ExecuteMethodByID(methodId, params, 6);
				[data release];
				//do we need to clear the picture too?
				PA_ClearVariable(&params[0]);
				PA_ClearVariable(&params[1]);
				PA_ClearVariable(&params[2]);
				PA_ClearVariable(&params[3]);
				PA_ClearVariable(&params[4]);
				PA_ClearVariable(&params[5]);
				
				//cleanup
				[[NSFileManager defaultManager]removeItemAtURL:exportFolderUrl error:nil];
				[cachesFolderUrl release];
				[uuid release];
				
				if(result.fType == eVK_Boolean)
				{
					char b = PA_GetBooleanVariable(result);
					if(b)
					{
						break;
					}
				}
			}
		}
	}
}

void generateUuid(C_TEXT &returnValue)
{
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= 1080
	returnValue.setUTF16String([[[NSUUID UUID]UUIDString]stringByReplacingOccurrencesOfString:@"-" withString:@""]);
#else
	CFUUIDRef uuid = CFUUIDCreate(kCFAllocatorDefault);
	NSString *uuid_str = (NSString *)CFUUIDCreateString(kCFAllocatorDefault, uuid);
	returnValue.setUTF16String([uuid_str stringByReplacingOccurrencesOfString:@"-" withString:@""]);
#endif
}

void setArrayParameter(PA_Variable *param, PackagePtr pParams, unsigned int i)
{
	PA_Variable *p = ((PA_Variable *)pParams[i-1]);
	p->fType = param->fType;
	p->fFiller = param->fFiller;
	p->uValue.fArray.fCurrent = param->uValue.fArray.fCurrent;
	p->uValue.fArray.fNbElements = param->uValue.fArray.fNbElements;
	p->uValue.fArray.fData = param->uValue.fArray.fData;
}

#pragma mark -

void Photos_GET_SELECTION(sLONG_PTR *pResult, PackagePtr pParams)
{
	//$1
	PA_Variable Param_selectedNames = *((PA_Variable*) pParams[0]);
	
	switch (Param_selectedNames.fType)
	{
		case eVK_ArrayPicture:
			break;
		case eVK_Undefined:
			PA_ClearVariable(&Param_selectedNames);
			Param_selectedNames = PA_CreateVariable(eVK_ArrayPicture);
			break;
		default:
			break;
	}
	
	PA_ResizeArray(&Param_selectedNames, 0);
	
	//$6
	C_LONGINT Param_processId;
	
	//$5
	C_TEXT Param_callbackMethodName;
	Param_callbackMethodName.fromParamAtIndex(pParams, 5);
	methodId = (method_id_t)PA_GetMethodID((PA_Unichar *)Param_callbackMethodName.getUTF16StringPtr());
	bool import_async = methodId;
	
	//$4, $3, $2
	ARRAY_TEXT Param_names;
	ARRAY_TEXT Param_dates;
	ARRAY_TEXT Param_ids;

	Param_names.setSize(1);
	Param_dates.setSize(1);
	Param_ids  .setSize(1);
	
	BOOL usingOriginals = YES;
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
	NSString *cachesFolder = [paths objectAtIndex:0];
	
	if(Photos)
	{
		if(!import_async)
		{
			NSArray *photos = [Photos selection];
			
			for(NSUInteger i = 0; i < [photos count]; ++i)
			{
				PA_YieldAbsolute();
				PhotosMediaItem *photo = (PhotosMediaItem *)[photos objectAtIndex:i];

				if((photo) && ([photo id]) && ([photo filename]) && ([photo date]))
				{
					Param_ids.appendUTF16String([photo id]);
					Param_names.appendUTF16String([photo filename]);
					Param_dates.appendUTF16String([[photo date]description]);
					
					//setup
					NSUUID *uuid = [[NSUUID alloc]init];
					NSString *folderName = [uuid UUIDString];
					NSURL *cachesFolderUrl = (NSURL *)CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)cachesFolder, kCFURLPOSIXPathStyle, true);
					
					//export
					NSURL *exportFolderUrl = [cachesFolderUrl URLByAppendingPathComponent:folderName isDirectory:true];
					[Photos export:[NSArray arrayWithObject:photo] to:exportFolderUrl usingOriginals:usingOriginals];
					NSURL *exportFileUrl;
					if(!usingOriginals)
					{
						//the extension is changed to jpg
						exportFileUrl = [exportFolderUrl URLByAppendingPathComponent:[[photo.filename stringByDeletingPathExtension]stringByAppendingString:@".jpg"] isDirectory:false];
					}else{
						exportFileUrl = [exportFolderUrl URLByAppendingPathComponent:photo.filename isDirectory:false];
					}
					
					NSData *data = [[NSData alloc]initWithContentsOfURL:exportFileUrl];
					
					if(data)
					{
						PA_Picture picture = PA_CreatePicture((void*)[data bytes], [data length]);
						PA_ResizeArray(&Param_selectedNames, i + 1);
						PA_SetPictureInArray(Param_selectedNames, i + 1, picture);
						[data release];
					}
					
					//cleanup
					[[NSFileManager defaultManager]removeItemAtURL:exportFolderUrl error:nil];
					[cachesFolderUrl release];
					[uuid release];
					
				}
			}
			
		}else{
			//async
			C_TEXT processName;
			generateUuid(processName);
			Param_processId.setIntValue(
			PA_NewProcess((void *)importImages,
										0,
										(PA_Unichar *)processName.getUTF16StringPtr()));
		}
	}
	setArrayParameter(&Param_selectedNames, pParams, 1);
	
	Param_names.toParamAtIndex(pParams, 2);
	Param_dates.toParamAtIndex(pParams, 3);
	Param_ids  .toParamAtIndex(pParams, 4);
	
	Param_processId.toParamAtIndex(pParams, 6);
}


