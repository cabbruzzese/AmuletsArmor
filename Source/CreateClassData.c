#include "CreateClassData.h"
#include "BitmapLoader.h"
#include "STATS.H"
#include "GRAPHICS.H"

CreateClassData* NewCreateClassData (T_byte8 classnum)
{
	CreateClassData *classdata = malloc(sizeof(CreateClassData));
	
	classdata->ClassNum = classnum;
	classdata->RegenHealthModifier = 1;
	classdata->RegenManaModifier = 1;
	classdata->JumpModifier = 1;
	classdata->DamageModifier = 1;
	classdata->ThiefModifier = 0;
	classdata->BasePunchDamage = 3;
	classdata->SpellSystem = SPELL_SYSTEM_MAGE;
	*classdata->LevelTitles = (T_byte8*)malloc(sizeof(T_byte8) * NUM_TITLES_PER_CLASS);

	return classdata;
}

E_Boolean CreateClassDatasLoaded()
{
	if (CreateClassDatas[0] == NULL)
		return FALSE;

	return TRUE;
}

T_bitmap* LoadImageFromRes(T_byte8 classnum)
{
	T_resource res;
    T_byte8 stmp[64];
    T_byte8 *p_data;

    DebugRoutine ("LoadImageFromRes");

    sprintf (stmp,"UI/CREATEC/CHAR%02d", classnum);
    if (PictureExist (stmp))
    {
        res=PictureFind(stmp);
        p_data=PictureLockQuick (res);
        PictureUnlockAndUnfind (res);
	}
	else
	{
		p_data = NULL;
	}

	DebugEnd();

	return PictureToBitmap(p_data);
}

T_bitmap* LoadImage(T_byte8 classnum)
{
	/*long fsize = 0;
	T_byte8* data;
	char filename[32];
	FILE *fp;

	DebugRoutine ("LoadImage");

	sprintf(filename, "./classes/%i.bmp", (int)classnum);
	if (fp = fopen(filename, "r"))
	{
		fsize = ftell(fp);
		data = (T_byte8*) malloc (sizeof(char)*fsize);
		fread(data, 1, fsize, fp);
		fclose(fp);
	}
	else
	{
		data = NULL;
	}
	DebugEnd();
	return data;*/

	BITMAPINFOHEADER bmpheader;
	T_byte8 *p_data, *f_data, *tempptr;
	T_bitmap *bmp_data = (T_bitmap*)malloc(sizeof(T_bitmap));
	T_bitmap *retval;
	int i = 0;

	char filename[32];

	DebugRoutine("LoadImage");

	sprintf(filename, "./classes/%i.bmp", (int)classnum);

	p_data = LoadBitmapFile(filename, &bmpheader);
	if (p_data != NULL)
	{
		bmp_data->sizex = (T_word16)bmpheader.biWidth;
		bmp_data->sizey = (T_word16)bmpheader.biHeight;
		f_data = (T_byte8*)malloc(bmpheader.biSizeImage + 2);
		tempptr = f_data;
		f_data[0] = (int)bmpheader.biWidth;
		f_data[1] = 0;
		f_data[2] = (int)bmpheader.biHeight;
		f_data[3] = 0;
		f_data+=4;
		for (i = bmpheader.biHeight; i > 0; i--)
		{
			memcpy(&f_data[(bmpheader.biHeight-i)*bmpheader.biWidth], 
				   &p_data[i*bmpheader.biWidth], 
				   bmpheader.biWidth);
		}
		retval = (T_bitmap *)tempptr;

		//for (i = 0; i < bmp_data->sizey; i++)
		//{
			//bmp_data->data[i] = (T_byte8)p_data + (i * bmp_data->sizex);
		//}

		
	}
	else
	{
		retval = NULL;
	}

	DebugEnd();

	return retval;
}

T_void ReadClassData(T_byte8 classnum)
{
	FILE *fp;
	char filename[32];
	char cbuffer = ' ';
	char sbuffer[MAXLEVELTITLELENGTH] = "";
	char title[MAXCLASSTITLELENGTH];
	char descr[MAXCLASSDESCRLENGTH] = "";
	char *levelTitles[NUM_TITLES_PER_CLASS];
	int attr[NUM_ATTRIBUTES];
	int adv[NUM_ATTRIBUTES];
	int i, j;
	int magic, canUseFlag;
	T_bitmap *pictureData;
	float hmod, manamod, jmod, dmod, movemod, tmod, pdmg;
	float num;

	DebugRoutine ("ReadClassData");

	sprintf(filename, "./classes/%i.dat", (int)classnum);
	if (fp = fopen(filename, "r"))
	{
		//title
		fscanf(fp, "%s", title);

		//description
		while(cbuffer != '~')
		{
			fscanf(fp, "%c", &cbuffer);

			//don't write delmiters
			if (cbuffer != '~')
				descr[strlen(descr)] = cbuffer;
		}
		fseek(fp, 2, SEEK_CUR);
		
		//attributes
		for (i = 0; i<NUM_ATTRIBUTES;i++)
		{
			fscanf(fp, "%f", &num);
			attr[i] = (int)num;
			fscanf(fp, "%c", &cbuffer);
		}
		fseek(fp, 2, SEEK_CUR);

		//advancement
		for (i = 0; i<NUM_ATTRIBUTES;i++)
		{
			fscanf(fp, "%f", &num);
			adv[i] = (int)num;
			fscanf(fp, "%c", &cbuffer);
		}
		fseek(fp, 2, SEEK_CUR);

		//level titles
		for (i = 0; i<NUM_TITLES_PER_CLASS; i++)
		{
			cbuffer = ' ';
			j = 0;
			while(cbuffer != ',' && cbuffer != '~')
			{
				fscanf(fp, "%c", &cbuffer);

				//don't write delmiters
				if (cbuffer != ',' && cbuffer != '~')
					sbuffer[j] = cbuffer;
				//end string with null val
				else
					sbuffer[j] = '\0';
				j++;
			}
			levelTitles[i] = (char*)malloc(sizeof(char));
			strcpy(levelTitles[i], sbuffer);
		}
		fseek(fp, 2, SEEK_CUR);

		//Magic System
		fscanf(fp, "%f", &num);
		magic = (int)num;
		fseek(fp, 1, SEEK_CUR);

		//CanUseBy Flag
		fscanf(fp, "%f", &num);
		canUseFlag = (int)num;
		if (canUseFlag == 0)
			canUseFlag = 1<<classnum;
		fseek(fp, 3, SEEK_CUR);

		//Health Regen Mod
		fscanf(fp, "%f", &num);
		hmod = num;
		fseek(fp, 1, SEEK_CUR);

		//Mana Regen Mod
		fscanf(fp, "%f", &num);
		manamod = num;
		fseek(fp, 1, SEEK_CUR);

		//Jump Mod
		fscanf(fp, "%f", &num);
		jmod = num;
		fseek(fp, 1, SEEK_CUR);

		//Damage Mod
		fscanf(fp, "%f", &num);
		dmod = num;
		fseek(fp, 1, SEEK_CUR);

		//Move Mod
		fscanf(fp, "%f", &num);
		movemod = num;
		fseek(fp, 1, SEEK_CUR);

		//Thief Mod
		fscanf(fp, "%f", &num);
		tmod = num;
		fseek(fp, 1, SEEK_CUR);

		//Base Punch Damage
		fscanf(fp, "%f", &num);
		pdmg = num;
		fseek(fp, 1, SEEK_CUR);
		fclose(fp);
	}

	strcpy(CreateClassDatas[classnum]->Title, title);
	strcpy(CreateClassDatas[classnum]->Descr, descr);
	for (i=0; i<NUM_ATTRIBUTES;i++)
	{
		CreateClassDatas[classnum]->Attributes[i] = attr[i];
		CreateClassDatas[classnum]->Advancement[i] = adv[i];
	}
	for (i=0; i<NUM_TITLES_PER_CLASS;i++)
	{
		CreateClassDatas[classnum]->LevelTitles[i] = (T_byte8*)malloc(sizeof(T_byte8));
		strcpy(CreateClassDatas[classnum]->LevelTitles[i], levelTitles[i]);
	}
	CreateClassDatas[classnum]->SpellSystem = magic;
	CreateClassDatas[classnum]->CanUseFlag = canUseFlag;
	CreateClassDatas[classnum]->RegenHealthModifier = hmod;
	CreateClassDatas[classnum]->RegenManaModifier = manamod;
	CreateClassDatas[classnum]->JumpModifier = jmod;
	CreateClassDatas[classnum]->DamageModifier = dmod;
	CreateClassDatas[classnum]->MoveModifier = movemod;
	CreateClassDatas[classnum]->ThiefModifier = tmod;
	CreateClassDatas[classnum]->BasePunchDamage = (int)pdmg;

	pictureData = LoadImage(classnum);
	if (pictureData == NULL)
		pictureData = LoadImageFromRes(classnum);

	CreateClassDatas[classnum]->Picture = pictureData;

	DebugEnd();
}

T_void LoadCreateClassDatas(T_void)
{
	T_byte8 i;

	for (i = 0; i < NUM_CLASSES; i++)
	{	
		//create and assign values
		CreateClassDatas[i] = NewCreateClassData(i);
		ReadClassData(i);		
	}
}

T_void DestroyCreateClassDatas(T_void)
{
	int i;
	for(i = 0; i < NUM_CLASSES; i++)
	{
		free(CreateClassDatas[i]);
	}
}