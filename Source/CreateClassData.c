#include "CreateClassData.h"
#include "BitmapLoader.h"
#include "STATS.H"
#include "GRAPHICS.H"

ItemListEntry *FindFromItemList(char *itemname)
{
	int i = 0;
	ItemListEntry *retvalue;

	DebugRoutine("FindFromItemList");

	retvalue = NULL;
	for (i = 0; i < ItemListCount; i++)
	{
		if (&ItemList[i] == NULL)
			continue;

		if (strcmp(itemname, ItemList[i].name) == 0)
			retvalue = &ItemList[i];
	}

	DebugEnd();

	return retvalue;
}

T_void LoadItemList()
{
	FILE *fp;
	char filename[32];
	char cbuffer = ' ';
	char name[128];
	float num = 0;
	int lines = 0;
	int i = 0;
	ItemListEntry *item;

	DebugRoutine ("LoadItemList");

	sprintf(filename, "./classes/itemlist.dat");
	if (fp = fopen(filename, "r"))
	{
		//count lines in order to allocate space
		while (!feof(fp))
		{
			fscanf(fp, "%c", &cbuffer);
			if (cbuffer == 10)
			{
				lines++;
			}
		}

		//size array
		// Always assume there's 1 extra entry because eof will terminate the last line
		ItemListCount = lines + 1;
		ItemList = (ItemListEntry*)malloc(sizeof(ItemListEntry) * ItemListCount);
		

		rewind(fp);
		//count lines in order to assign space
		cbuffer = ' ';
		while (!feof(fp))
		{
			//read values
			fscanf(fp, "%s", &name);
			//skip tab
			fseek(fp, 1, SEEK_CUR);
			fscanf(fp, "%f", &num);

			//create new item
			item = (ItemListEntry*)malloc(sizeof(ItemListEntry));
			item->name = (char*)malloc(sizeof(char));
			strcpy(item->name, name);
			item->num = (T_word16)num;
			//add to list
			ItemList[i] = *item;

			i++;

			//read next char data
			fscanf(fp, "%c", &cbuffer);
		}	

	}

	DebugEnd();
}

CreateClassData* NewCreateClassData (T_byte8 classnum)
{
	CreateClassData *classdata = malloc(sizeof(CreateClassData));

    DebugRoutine ("NewCreateClassData");

	classdata->ClassNum = classnum;
	classdata->RegenHealthModifier = 1;
	classdata->RegenManaModifier = 1;
	classdata->JumpModifier = 1;
	classdata->DamageModifier = 1;
	classdata->ThiefModifier = 0;
	classdata->BasePunchDamage = 3;
	classdata->SpellSystem = SPELL_SYSTEM_MAGE;
	*classdata->LevelTitles = (T_byte8*)malloc(sizeof(T_byte8) * NUM_TITLES_PER_CLASS);

	DebugEnd();

	return classdata;
}

E_Boolean CreateClassDatasLoaded()
{
	E_Boolean retvalue = TRUE;

	DebugRoutine ("CreateClassDatasLoaded");

	if (CreateClassDatas[0] == NULL)
		retvalue = FALSE;

	DebugEnd();

	return retvalue;
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
	char itemname[128];
	float itemcount = 0;
	long temppos = 0;
	int itemscount = 0;
	StartingItemData *item;
	int attr[NUM_ATTRIBUTES];
	int adv[NUM_ATTRIBUTES];
	int i, j;
	int magic, canUseFlag;
	T_bitmap *pictureData;
	float num;
	float  hmod, manamod, jmod, dmod, movemod, tmod, pdmg;

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
		fseek(fp, 3, SEEK_CUR);

		//canuse weapons
		for (i = 0; i < (EQUIP_WEAPON_TYPE_UNKNOWN - 1); i++)
		{
			fscanf(fp, "%f", &num);
			CreateClassDatas[classnum]->CanUseWeapon[i] = (E_Boolean)num;
			fseek(fp, 1, SEEK_CUR);
		}
		fseek(fp, 2, SEEK_CUR);

		//canuse armor
		for (i = 0; i < NUM_ARMOR_TYPES; i++)
		{
			fscanf(fp, "%f", &num);
			CreateClassDatas[classnum]->CanUseArmor[i] = (E_Boolean)num;
			fseek(fp, 1, SEEK_CUR);
		}
		fseek(fp, 2, SEEK_CUR);

		//Starting Items List
		{
			//get total number of weapons to be used
			temppos = ftell(fp);
			cbuffer = ' ';
			while(cbuffer != '~')
			{
				fscanf(fp, "%c", &cbuffer);
				if (cbuffer == ',')
				{
					itemscount++;
				}
			}

			//start item list
			CreateClassDatas[classnum]->StartingItemsCount = itemscount + 1;
			CreateClassDatas[classnum]->StartingItems = (StartingItemData*)malloc(sizeof(StartingItemData) * CreateClassDatas[classnum]->StartingItemsCount);

			//rewind
			fseek(fp, temppos, SEEK_SET);

			//read in numbers
			cbuffer = ' ';
			itemscount = 0;
			while(cbuffer != '~')
			{
				//read values
				fscanf(fp, "%s", &itemname);
				fscanf(fp, "%f", &itemcount);

				//create new item entry
				item = (StartingItemData*)malloc(sizeof(ItemListEntry));
				item->num = FindFromItemList(itemname)->num;
				item->count = (T_byte8)itemcount;

				//assign entry
				CreateClassDatas[classnum]->StartingItems[itemscount] = *item;

				fscanf(fp, "%c", &cbuffer);
				//if another entry follows
				if (cbuffer == ',')
				{
					//skip newline char
					fseek(fp, 2, SEEK_CUR);
					itemscount++;
				}
			}
		}

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

	DebugRoutine("LoadCreateClassDatas");

	LoadItemList();

	for (i = 0; i < NUM_CLASSES; i++)
	{	
		//create and assign values
		CreateClassDatas[i] = NewCreateClassData(i);
		ReadClassData(i);		
	}

	DebugEnd();
}

T_void DestroyCreateClassDatas(T_void)
{
	int i;

	DebugRoutine("DestroyCreateClassDatas");

	for(i = 0; i < NUM_CLASSES; i++)
	{
		free(CreateClassDatas[i]);
	}

	DebugEnd();
}