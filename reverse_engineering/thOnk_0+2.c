// Pseudocódigo exportado pelo Ghidra
// Programa: thOnk_0+2.app
// Não é o código-fonte original.


/* Função: FUN_10000000 | Endereço: 10000000 */

void FUN_10000000(void)

{
  return;
}




/* Função: .AIFFCorrectHeader | Endereço: 1000001c */

void .debug::_AIFFCorrectHeader(undefined4 param_1)

{
  short sVar1;
  uint uVar2;
  int iVar3;
  undefined1 auStack_28 [12];
  short local_1c;
  undefined4 local_18;
  int local_14 [3];
  
  sVar1 = .glue::GetFPos(param_1,&local_18);
  if ((((sVar1 == 0) && (sVar1 = .glue::SetFPos(param_1,2,0), sVar1 == 0)) &&
      (sVar1 = .glue::GetFPos(param_1,local_14), sVar1 == 0)) &&
     (sVar1 = _GetAIFFInfo(param_1,auStack_28), sVar1 == 0)) {
    uVar2 = local_14[0] - 0x42;
    iVar3 = (int)(((int)uVar2 >> 1) + (uint)((int)uVar2 < 0 && (uVar2 & 1) != 0)) / (int)local_1c;
    .glue::SetFPos(param_1,1,0);
    _FindID(0x464f524d,param_1);
    _PutLong(local_14[0] + -8,param_1);
    _FindID(0x434f4d4d,param_1);
    .glue::SetFPos(param_1,3,6);
    _PutLong(iVar3,param_1);
    _FindID(0x53534e44,param_1);
    _PutLong(iVar3 * 2 * (int)local_1c + 8,param_1);
  }
  .glue::SetFPos(param_1,1,local_18);
  return;
}




/* Função: .AIFFSetEOF | Endereço: 100001bc */

undefined4 .debug::_AIFFSetEOF(undefined4 param_1,int param_2)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int iVar4;
  int local_5c;
  undefined4 local_38;
  undefined1 auStack_34 [12];
  short local_28;
  int local_24;
  int local_20;
  int local_1c [2];
  
  uVar2 = _GetAIFFInfo(param_1,auStack_34);
  if (((short)uVar2 == 0) && (uVar2 = .glue::GetFPos(param_1,&local_38), (short)uVar2 == 0)) {
    iVar1 = (int)local_28;
    .glue::GetEOF(param_1,local_1c);
    if (DAT_1000c668 != '\0') {
      _FindBeginOfSoundData(param_1);
      .glue::GetFPos(param_1,local_5c + 0x3b4);
      DAT_1000c668 = '\0';
    }
    iVar4 = *(int *)(local_5c + 0x3b4) + iVar1 * 2 * param_2;
    if ((iVar4 < local_1c[0]) || (iVar4 <= *(int *)(local_5c + 0x3b4))) {
      uVar2 = 0xffffffdc;
    }
    else {
      uVar3 = .glue::NewPtrClear(iVar1 << 0xc);
      uVar2 = .glue::MemError();
      if ((short)uVar2 == 0) {
        local_24 = iVar1 << 0xc;
        local_20 = local_1c[0];
        while ((local_20 < iVar4 && ((short)uVar2 == 0))) {
          uVar2 = .glue::SetFPos(param_1,1,local_20);
          if ((short)uVar2 == 0) {
            uVar2 = .glue::FSWrite(param_1,&local_24,uVar3);
          }
          if ((short)uVar2 == 0) {
            uVar2 = .glue::GetFPos(param_1,&local_20);
          }
        }
        if ((short)uVar2 != 0) {
          uVar2 = .glue::SetEOF(param_1,iVar4);
        }
        .glue::DisposePtr(uVar3);
      }
    }
  }
  if ((short)uVar2 == 0) {
    uVar2 = .glue::SetFPos(param_1,1,local_38);
  }
  return uVar2;
}




/* Função: .AIFFSndAdd | Endereço: 100003c8 */

undefined4
.debug::_AIFFSndAdd(undefined4 param_1,int param_2,int param_3,short param_4,int param_5,
                   int *param_6)

{
  short *psVar1;
  short sVar3;
  int iVar2;
  short sVar4;
  int iVar5;
  int iVar6;
  short sVar7;
  int iVar8;
  undefined4 uVar9;
  int iVar10;
  int local_5c;
  uint local_38 [3];
  
  uVar9 = 0;
  if (DAT_1000c278 != '\0') {
    _FindBeginOfSoundData(param_1);
    .glue::GetFPos(param_1,&DAT_1000c280);
    uVar9 = _GetAIFFInfo(param_1,&DAT_1000d3d8);
    *(undefined1 *)(local_5c + 0x3b8) = 0;
  }
  sVar4 = sRam1000d3e4;
  if ((short)uVar9 == 0) {
    sVar3 = sRam1000d3e4 << 1;
    local_38[0] = param_3 * sVar3;
    iVar6 = .glue::NewPtrClear(local_38[0]);
    uVar9 = .glue::MemError();
    if ((short)uVar9 == 0) {
      iVar5 = param_5 * sVar3;
      iVar10 = *(int *)(local_5c + 0x3c0) + iVar5;
      sVar7 = .glue::SetFPos(param_1,3,iVar5);
      if (sVar7 == -0x27) {
        _AIFFSetEOF(param_1,param_5 + param_3);
      }
      .glue::SetFPos(param_1,1,iVar10);
      sVar7 = .glue::FSRead(param_1,local_38,iVar6);
      if (sVar7 == -0x27) {
        local_38[0] = param_3 * sVar3;
      }
      iVar8 = 0;
      iVar5 = 0;
      if (0 < param_3) {
        do {
          psVar1 = (short *)(param_2 + iVar5);
          iVar2 = ((int)(short)(param_4 + -1) + iVar8 * sVar4) * 2;
          iVar8 = iVar8 + 1;
          iVar5 = iVar5 + 2;
          *(short *)(iVar6 + iVar2) = *(short *)(iVar6 + iVar2) + *psVar1;
          param_3 = param_3 + -1;
        } while (param_3 != 0);
      }
      .glue::SetFPos(param_1,1,iVar10);
      uVar9 = .glue::FSWrite(param_1,local_38,iVar6);
      *param_6 = ((int)local_38[0] >> 1) + (uint)((int)local_38[0] < 0 && (local_38[0] & 1) != 0);
      .glue::DisposePtr(iVar6);
    }
  }
  return uVar9;
}




/* Função: .AIFFSndAddStereo | Endereço: 100005d4 */

undefined4
.debug::_AIFFSndAddStereo
          (undefined4 param_1,int param_2,int param_3,int param_4,int param_5,int *param_6)

{
  short *psVar1;
  short sVar3;
  int iVar2;
  int iVar4;
  int iVar5;
  short sVar6;
  undefined4 uVar7;
  int iVar8;
  int local_5c;
  uint local_38 [4];
  
  uVar7 = 0;
  if (DAT_1000c279 != '\0') {
    _FindBeginOfSoundData(param_1);
    .glue::GetFPos(param_1,&DAT_1000c27c);
    uVar7 = _GetAIFFInfo(param_1,&DAT_1000d3c8);
    *(undefined1 *)(local_5c + 0x3b9) = 0;
  }
  if ((short)uVar7 == 0) {
    sVar3 = sRam1000d3d4 << 1;
    local_38[0] = param_4 * sVar3;
    iVar5 = .glue::NewPtrClear(local_38[0]);
    uVar7 = .glue::MemError();
    if ((short)uVar7 == 0) {
      iVar4 = param_5 * sVar3;
      iVar8 = *(int *)(local_5c + 0x3bc) + iVar4;
      sVar6 = .glue::SetFPos(param_1,3,iVar4);
      if (sVar6 == -0x27) {
        _AIFFSetEOF(param_1,param_5 + param_4);
      }
      .glue::SetFPos(param_1,1,iVar8);
      sVar6 = .glue::FSRead(param_1,local_38,iVar5);
      if (sVar6 == -0x27) {
        local_38[0] = param_4 * sVar3;
      }
      iVar4 = 0;
      if (0 < param_4) {
        do {
          iVar2 = (iVar4 + 1) * 2;
          *(short *)(iVar5 + iVar4 * 2) =
               *(short *)(iVar5 + iVar4 * 2) + *(short *)(param_2 + iVar4);
          psVar1 = (short *)(param_3 + iVar4);
          iVar4 = iVar4 + 2;
          *(short *)(iVar5 + iVar2) = *(short *)(iVar5 + iVar2) + *psVar1;
          param_4 = param_4 + -1;
        } while (param_4 != 0);
      }
      .glue::SetFPos(param_1,1,iVar8);
      uVar7 = .glue::FSWrite(param_1,local_38,iVar5);
      *param_6 = ((int)local_38[0] >> 1) + (uint)((int)local_38[0] < 0 && (local_38[0] & 1) != 0);
      .glue::DisposePtr(iVar5);
    }
  }
  return uVar7;
}




/* Função: .AIFFSndRead | Endereço: 100007e4 */

undefined4
.debug::_AIFFSndRead
          (undefined4 param_1,undefined4 param_2,int param_3,undefined4 param_4,int param_5,
          int *param_6)

{
  bool bVar1;
  short sVar2;
  char cVar3;
  undefined4 uVar4;
  int local_5c;
  uint local_38 [5];
  
  uVar4 = 0;
  if (DAT_1000c284 != '\0') {
    _GetAIFFInfo(param_1,&DAT_1000d3e8);
    DAT_1000c284 = '\0';
    cVar3 = _FindID(0x53534e44,param_1);
    if (cVar3 == '\0') {
      uVar4 = 0xffffd8f0;
    }
    else {
      uVar4 = .glue::GetFPos(param_1,&DAT_1000c288);
      *(int *)(local_5c + 0x3c8) = *(int *)(local_5c + 0x3c8) + -0xc;
    }
  }
  if ((short)uVar4 == 0) {
    bVar1 = param_5 + param_3 <= iRam1000d3f0;
    sVar2 = sRam1000d3f4 << 1;
    if (bVar1) {
      local_38[0] = param_3 * sVar2;
    }
    else {
      local_38[0] = iRam1000d3f0 - param_5;
    }
    uVar4 = .glue::MemError();
    if ((short)uVar4 == 0) {
      .glue::SetFPos(param_1,1,*(int *)(local_5c + 0x3c8) + param_5 * sVar2);
      uVar4 = .glue::FSRead(param_1,local_38,param_2);
      if (((short)uVar4 == 0) && (!bVar1)) {
        uVar4 = 0xffffffd9;
      }
      *param_6 = ((int)local_38[0] >> 1) + (uint)((int)local_38[0] < 0 && (local_38[0] & 1) != 0);
    }
  }
  return uVar4;
}




/* Função: .Ext80ToDbl | Endereço: 10000978 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

double .debug::_Ext80ToDbl(uint param_1,uint param_2,uint param_3)

{
  int iVar1;
  int iVar2;
  short sVar3;
  double dVar4;
  double dVar5;
  double dVar6;
  double dVar7;
  short sStack00000018;
  short sStack0000001a;
  uint uStack0000001c;
  uint uStack00000020;
  undefined4 local_7c;
  
  if ((param_1 & 0x80000000) == 0) {
    sVar3 = 1;
  }
  else {
    sVar3 = -1;
  }
  _sStack00000018 = param_1 & 0x7fffffff;
  iVar2 = (int)sStack0000001a;
  uStack0000001c._2_2_ = (short)param_2;
  iVar1 = (int)uStack0000001c._2_2_;
  uStack0000001c = param_2;
  uStack00000020 = param_3;
  dVar4 = .glue::pow(_DAT_1000c2b8,
                     (double)CONCAT44(0x43300000,(int)sStack00000018 - 0x401eU ^ 0x80000000) -
                     _DAT_1000c2b0);
  dVar7 = *(double *)(local_7c + 0x3f0);
  dVar6 = *(double *)(local_7c + 1000);
  dVar5 = .glue::pow(*(double *)(local_7c + 0x3f8),
                     (double)CONCAT44(0x43300000,(int)sStack00000018 - 0x403eU ^ 0x80000000) - dVar7
                    );
  return ((double)CONCAT44(0x43300000,(int)sVar3 ^ 0x80000000U) - *(double *)(local_7c + 0x3f0)) *
         ((double)CONCAT44(0x43300000,iVar1 * 0x10000 + (param_3 >> 0x10)) -
         *(double *)(local_7c + 1000)) * dVar5 +
         ((double)CONCAT44(0x43300000,(int)sVar3 ^ 0x80000000U) - dVar7) *
         ((double)CONCAT44(0x43300000,iVar2 * 0x10000 + (param_2 >> 0x10)) - dVar6) * dVar4;
}




/* Função: .DblToExt80 | Endereço: 10000af8 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_DblToExt80(double param_1,undefined4 *param_2)

{
  uint uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  ushort uVar4;
  double dVar5;
  double dVar6;
  double dVar7;
  double dVar8;
  double dVar9;
  int local_8c;
  undefined4 local_60;
  
  if (_DAT_1000c2a0 <= param_1) {
    uVar4 = 0;
  }
  else {
    uVar4 = 0x8000;
  }
  if (uVar4 != 0) {
    param_1 = -param_1;
  }
  dVar5 = .glue::log(_DAT_1000c2b8);
  dVar6 = .glue::log(param_1);
  uVar1 = (int)(dVar6 / dVar5) & 0xffff;
  dVar7 = .glue::pow(*(double *)(local_8c + 0x3f8),
                     ((double)CONCAT44(0x43300000,uVar1) - *(double *)(local_8c + 1000)) -
                     *(double *)(local_8c + 0x3d8));
  uVar2 = FUN_10009774(param_1 / dVar7);
  dVar7 = .glue::pow(*(double *)(local_8c + 0x3f8),
                     ((double)CONCAT44(0x43300000,uVar1) - *(double *)(local_8c + 1000)) -
                     *(double *)(local_8c + 0x3d8));
  dVar9 = *(double *)(local_8c + 1000);
  dVar8 = .glue::pow(*(double *)(local_8c + 0x3f8),
                     *(double *)(local_8c + 0x3d0) + ((double)CONCAT44(0x43300000,uVar1) - dVar9));
  uVar3 = FUN_10009774(-(((double)CONCAT44(0x43300000,uVar2) - dVar9) * dVar7 - param_1) / dVar8);
  local_60 = CONCAT22((short)(int)(dVar6 / dVar5) + 0x3fffU | uVar4,(short)((uint)uVar2 >> 0x10));
  *param_2 = local_60;
  param_2[1] = CONCAT22((short)uVar2,(short)((uint)uVar3 >> 0x10));
  *(short *)(param_2 + 2) = (short)uVar3;
  return;
}




/* Função: .CloseAIFFSourceFile | Endereço: 10000c90 */

void .debug::_CloseAIFFSourceFile(void)

{
  .glue::FSClose();
  return;
}




/* Função: .CloseAIFFDestinationFile | Endereço: 10000cdc */

void .debug::_CloseAIFFDestinationFile(undefined4 param_1)

{
  short sVar1;
  
  sVar1 = _AIFFCorrectHeader(param_1);
  if (sVar1 == 0) {
    .glue::FSClose(param_1);
  }
  return;
}




/* Função: .InitAIFFTools | Endereço: 10000d54 */

void .debug::_InitAIFFTools(void)

{
  undefined4 local_2c;
  
  .glue::MaxApplZone();
  .glue::MoreMasters();
  .glue::MoreMasters();
  .glue::InitGraf(local_2c + 0x1c5a);
  .glue::InitFonts();
  .glue::InitWindows();
  .glue::InitMenus();
  .glue::TEInit();
  .glue::InitDialogs(0);
  .glue::FlushEvents(0xffff,0);
  .glue::InitCursor();
  return;
}




/* Função: .GetExtended | Endereço: 10000e00 */

void .debug::_GetExtended(undefined4 param_1)

{
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c [3];
  
  local_c[0] = 10;
  .glue::FSRead(param_1,local_c,&local_18);
  _Ext80ToDbl(local_18,local_14,local_10);
  return;
}




/* Função: .GetLong | Endereço: 10000e68 */

undefined4 .debug::_GetLong(undefined4 param_1)

{
  undefined4 local_8;
  undefined4 local_4;
  
  local_4 = 4;
  .glue::FSRead(param_1,&local_4,&local_8);
  return local_8;
}




/* Função: .GetInt | Endereço: 10000ebc */

int .debug::_GetInt(undefined4 param_1)

{
  undefined4 local_8;
  short local_4 [2];
  
  local_8 = 2;
  .glue::FSRead(param_1,&local_8,local_4);
  return (int)local_4[0];
}




/* Função: .FindID | Endereço: 10000f10 */

bool .debug::_FindID(int param_1,undefined4 param_2)

{
  bool bVar1;
  int iVar2;
  short sVar4;
  undefined4 uVar3;
  
  .glue::SetFPos(param_2,1,0);
  if (param_1 == 0x464f524d) {
    iVar2 = _GetLong(param_2);
    bVar1 = iVar2 != 0x464f524d;
    .glue::SetFPos(param_2,3,0);
  }
  else {
    sVar4 = .glue::SetFPos(param_2,1,0xc);
    while ((sVar4 != -0x27 && (iVar2 = _GetLong(param_2), param_1 != iVar2))) {
      uVar3 = _GetLong(param_2);
      sVar4 = .glue::SetFPos(param_2,3,uVar3);
    }
    bVar1 = sVar4 == -0x27;
    if (!bVar1) {
      .glue::SetFPos(param_2,3,0);
    }
  }
  return !bVar1;
}




/* Função: .GetAIFFInfo | Endereço: 10001058 */

void .debug::_GetAIFFInfo(undefined4 param_1,float *param_2)

{
  char cVar3;
  undefined2 uVar2;
  float fVar1;
  double dVar4;
  undefined4 local_18 [4];
  
  .glue::GetFPos(param_1,local_18);
  cVar3 = _FindID(0x434f4d4d,param_1);
  if (cVar3 == '\0') {
    *(undefined1 *)((int)param_2 + 0xe) = 0;
  }
  else {
    .glue::SetFPos(param_1,3,4);
    uVar2 = _GetInt(param_1);
    *(undefined2 *)(param_2 + 3) = uVar2;
    fVar1 = (float)_GetLong(param_1);
    param_2[2] = fVar1;
    uVar2 = _GetInt(param_1);
    *(undefined2 *)(param_2 + 1) = uVar2;
    dVar4 = (double)_GetExtended(param_1);
    *param_2 = (float)dVar4;
    if (*(short *)(param_2 + 1) == 0x10) {
      *(undefined1 *)((int)param_2 + 0xe) = 1;
    }
    else {
      *(undefined1 *)((int)param_2 + 0xe) = 0;
    }
  }
  .glue::SetFPos(param_1,1,local_18[0]);
  return;
}




/* Função: .FindBeginOfSoundData | Endereço: 10001160 */

bool .debug::_FindBeginOfSoundData(undefined4 param_1)

{
  char cVar1;
  
  cVar1 = _FindID(0x53534e44,param_1);
  if (cVar1 != '\0') {
    .glue::SetFPos(param_1,3,0xc);
  }
  return cVar1 != '\0';
}




/* Função: .NewAIFFDestinationFile | Endereço: 100011f4 */

undefined4
.debug::_NewAIFFDestinationFile
          (undefined8 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
          undefined4 param_5,undefined4 param_6)

{
  undefined4 uVar1;
  undefined6 uVar2;
  undefined1 auStack_78 [10];
  undefined1 auStack_6e [82];
  
  uVar2 = _AIFFPutFile(param_4,auStack_78);
  if ((short)((uint6)uVar2 >> 0x20) == 0) {
    uVar1 = _AIFFMakeFile(param_1,param_2,(int)uVar2,auStack_78,param_5,param_6);
  }
  else {
    uVar1 = 0xffffffdb;
  }
  .glue::BlockMove(auStack_6e,&DAT_1000d50c,0x20);
  return uVar1;
}




/* Função: .AIFFPutFile | Endereço: 100012c8 */

undefined4 .debug::_AIFFPutFile(undefined4 param_1,char *param_2)

{
  undefined4 uVar1;
  int local_3c;
  undefined4 local_18 [4];
  
  .glue::SetPt(local_18,0x32,0x32);
  .glue::SFPutFile(local_18[0],local_3c + 0x7a9,param_1,0,param_2);
  if (*param_2 == '\0') {
    uVar1 = 0xffffffdc;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}




/* Função: .AIFFMakeFile | Endereço: 10001364 */

void .debug::_AIFFMakeFile
               (undefined8 param_1,undefined4 param_2,undefined4 param_3,int param_4,
               undefined4 param_5,short *param_6)

{
  short sVar1;
  
  sVar1 = .glue::Create(param_4 + 10,(int)*(short *)(param_4 + 6),param_5,0x41494646);
  if ((sVar1 == -0x30) &&
     (sVar1 = .glue::FSDelete(param_4 + 10,(int)*(short *)(param_4 + 6)), sVar1 == 0)) {
    sVar1 = .glue::Create(param_4 + 10,(int)*(short *)(param_4 + 6),param_5,0x41494646);
  }
  if (((sVar1 == 0) &&
      (sVar1 = .glue::FSOpen(param_4 + 10,(int)*(short *)(param_4 + 6),param_6), sVar1 == 0)) &&
     (sVar1 = _WriteAIFFHeader(param_1,(int)*param_6,param_2), sVar1 != 0)) {
    .glue::FSClose((int)*param_6);
    .glue::FSDelete(param_4 + 10,(int)*(short *)(param_4 + 6));
  }
  return;
}




/* Função: .AIFFGetFile | Endereço: 100014a4 */

undefined4 .debug::_AIFFGetFile(char *param_1,undefined4 param_2)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int local_4c;
  undefined4 local_28 [4];
  undefined4 local_18 [3];
  
  uVar2 = 0;
  uVar1 = .glue::NewRoutineDescriptor(param_2,0xd0,1);
  local_28[0] = 0x41494646;
  .glue::SetPt(local_18,0x32,0x32);
  .glue::SFGetFile(local_18[0],local_4c + 0x400,uVar1,1,local_28,0,param_1);
  if (*param_1 == '\0') {
    uVar2 = 0xffffffdc;
  }
  if ((short)uVar2 == 0) {
    .glue::BlockMove(param_1 + 10,local_4c + 0x154c,(byte)param_1[10] + 1);
  }
  .glue::DisposeRoutineDescriptor(uVar1);
  return uVar2;
}




/* Função: .AIFFOpenFile | Endereço: 100015a0 */

void .debug::_AIFFOpenFile(int param_1,short *param_2,undefined4 param_3)

{
  short sVar1;
  
  sVar1 = .glue::FSOpen(param_1 + 10,(int)*(short *)(param_1 + 6),param_2);
  if (sVar1 == 0) {
    _GetAIFFInfo((int)*param_2,param_3);
  }
  return;
}




/* Função: .OpenAIFFSourceFile | Endereço: 10001624 */

void .debug::_OpenAIFFSourceFile(undefined4 param_1)

{
  short sVar1;
  undefined1 auStack_68 [76];
  undefined1 auStack_1c [24];
  
  sVar1 = _AIFFGetFile(auStack_68,0);
  if (sVar1 == 0) {
    _AIFFOpenFile(auStack_68,param_1,auStack_1c);
  }
  return;
}




/* Função: .PutExtended | Endereço: 1000169c */

void .debug::_PutExtended(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined1 auStack_18 [12];
  undefined4 local_c [2];
  
  _DblToExt80(auStack_18);
  local_c[0] = 10;
  .glue::FSWrite(param_3,local_c,auStack_18);
  return;
}




/* Função: .PutLong | Endereço: 1000170c */

void .debug::_PutLong(undefined4 param_1,undefined4 param_2)

{
  undefined4 local_8;
  undefined4 local_4;
  
  local_8 = 4;
  local_4 = param_1;
  .glue::FSWrite(param_2,&local_8,&local_4);
  return;
}




/* Função: .PutInt | Endereço: 10001764 */

void .debug::_PutInt(undefined2 param_1,undefined4 param_2)

{
  undefined4 local_8;
  undefined2 local_4 [2];
  
  local_8 = 2;
  local_4[0] = param_1;
  .glue::FSWrite(param_2,&local_8,local_4);
  return;
}




/* Função: .WriteAIFFHeader | Endereço: 100017bc */

void .debug::_WriteAIFFHeader(undefined8 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined8 uVar1;
  
  _PutLong(0x464f524d,param_2);
  _PutLong(0x73697a65,param_2);
  _PutLong(0x41494646,param_2);
  _PutLong(0x4150504c,param_2);
  _PutLong(4,param_2);
  _PutLong(0x47444d47,param_2);
  _PutLong(0x434f4d4d,param_2);
  _PutLong(0x12,param_2);
  _PutInt(param_3,param_2);
  _PutLong(0,param_2);
  uVar1 = _PutInt(0x10,param_2);
  _PutExtended(param_1,(int)((ulonglong)uVar1 >> 0x20),(int)uVar1,param_2);
  _PutLong(0x53534e44,param_2);
  _PutLong(8,param_2);
  _PutLong(0,param_2);
  _PutLong(0,param_2);
  return;
}




/* Função: .FindLinParm | Endereço: 100018fc */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

double .debug::_FindLinParm(double param_1,undefined4 param_2,undefined4 param_3,double *param_4)

{
  double *pdVar1;
  double *pdVar2;
  double dVar3;
  
  if (*param_4 <= param_1) {
    pdVar1 = (double *)_GetInsertBP(param_1);
    pdVar2 = *(double **)(pdVar1 + 2);
    if (pdVar2 == (double *)0x0) {
      dVar3 = pdVar1[1];
    }
    else {
      dVar3 = *pdVar2 - *pdVar1;
      if (*pdVar2 - *pdVar1 == _DAT_1000c2d0) {
        dVar3 = _DAT_1000c6a0;
      }
      dVar3 = ((param_1 - *pdVar1) / dVar3) * (pdVar2[1] - pdVar1[1]) + pdVar1[1];
    }
  }
  else {
    dVar3 = param_4[1];
  }
  return dVar3;
}




/* Função: .FindExpParm | Endereço: 100019b8 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

double .debug::_FindExpParm(double param_1,undefined4 param_2,undefined4 param_3,double *param_4)

{
  double *pdVar1;
  double dVar2;
  double dVar3;
  double dVar4;
  double dVar5;
  
  if (*param_4 <= param_1) {
    pdVar1 = (double *)_GetInsertBP(param_1);
    dVar3 = *pdVar1;
    dVar5 = (*(double **)(pdVar1 + 2))[1];
    dVar2 = pdVar1[1];
    if (_DAT_1000c2d0 == pdVar1[1]) {
      dVar2 = _DAT_1000c2c8;
    }
    if (_DAT_1000c2d0 == dVar5) {
      dVar5 = _DAT_1000c2c8;
    }
    if (*(int *)(pdVar1 + 2) != 0) {
      dVar4 = **(double **)(pdVar1 + 2) - dVar3;
      if (dVar4 == _DAT_1000c2d0) {
        dVar4 = _DAT_1000c6a0;
      }
      dVar2 = .glue::log(dVar2);
      dVar5 = .glue::log(dVar5);
      dVar2 = (double).glue::exp(((param_1 - dVar3) / dVar4) * (dVar5 - dVar2) + dVar2);
    }
  }
  else {
    dVar2 = param_4[1];
  }
  return dVar2;
}




/* Função: .BreakPointsToSoundFile | Endereço: 10001abc */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_BreakPointsToSoundFile(double param_1,double param_2)

{
  double dVar1;
  double dVar2;
  double dVar3;
  undefined **ppuVar4;
  undefined4 uVar5;
  short sVar6;
  undefined4 extraout_r4;
  double dVar7;
  double dVar8;
  double extraout_f1;
  double dVar9;
  double dVar10;
  double dVar11;
  double dVar12;
  double dVar13;
  ulonglong uVar14;
  undefined **local_ac;
  
  dVar12 = _DAT_1000c300;
  sVar6 = 0;
  uVar14 = _ShiftFirstBreakPoints();
  dVar3 = _DAT_1000c300;
  dVar2 = _DAT_1000c2f0;
  dVar1 = _DAT_1000c2e8;
  ppuVar4 = &.TVect::InitGraf;
  dVar11 = param_1;
  for (dVar13 = param_1; dVar13 <= param_2; dVar13 = dVar13 + _DAT_1000c6a0) {
    dVar7 = (double)_FindLinParm(dVar13,(int)(uVar14 >> 0x20),(int)uVar14,_DAT_1000d680);
    _FindGranFreq(dVar13);
    dVar8 = (double)_AbsDouble();
    if (dVar3 == dVar8) {
      dVar8 = *(double *)(ppuVar4 + 0x10e);
    }
    dVar12 = dVar8 * _DAT_1000c6a0 + dVar12;
    uVar5 = .glue::ceil(dVar12);
    uVar14 = CONCAT44(uVar5,extraout_r4);
    if (dVar2 <= extraout_f1) {
      dVar9 = extraout_f1;
      while (((dVar10 = dVar9 - dVar2, dVar9 != dVar3 && (DAT_1000d670 == '\0')) && (sVar6 == 0))) {
        dVar9 = (double)_FRand();
        sVar6 = _PutGrain(dVar11,dVar7 * dVar9 - dVar1 * dVar7,param_1);
        dVar11 = dVar11 + dVar2 / dVar8;
        uVar14 = .glue::Button();
        dVar9 = dVar10;
        if ((uVar14 & 0xff00000000) != 0) {
          uVar14 = _UpdateConsole(*(undefined8 *)(local_ac + 0x108),
                                  *(undefined8 *)(local_ac + 0x110),
                                  *(undefined8 *)(local_ac + 0x110));
        }
      }
      dVar12 = dVar12 - extraout_f1;
    }
    if ((DAT_1000d670 != '\0') || (sVar6 != 0)) {
      dVar13 = *(double *)(local_ac + 0x106);
    }
    ppuVar4 = local_ac;
  }
  return;
}




/* Função: .PutGrain | Endereço: 10001cf0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 .debug::_PutGrain(double param_1,double param_2,double param_3)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  undefined8 uVar3;
  double dVar4;
  int local_6c;
  
  puVar1 = (undefined4 *).glue::NewPtrClear(0x44);
  uVar2 = .glue::MemError();
  if ((short)uVar2 == 0) {
    uVar2 = _FindGrainlength(param_1);
    *puVar1 = uVar2;
    uVar3 = _FindAttack(param_1);
    *(undefined8 *)(puVar1 + 1) = uVar3;
    uVar3 = _FindDecay(param_1);
    *(undefined8 *)(puVar1 + 3) = uVar3;
    uVar3 = _FindGain(param_1);
    *(undefined8 *)(puVar1 + 5) = uVar3;
    uVar3 = _FindFrequencyMultiplier(param_1);
    *(undefined8 *)(puVar1 + 7) = uVar3;
    uVar3 = _FindBalance(param_1);
    *(undefined8 *)(puVar1 + 9) = uVar3;
    *(undefined2 *)(puVar1 + 0xb) = *(undefined2 *)(local_6c + 0x17ba);
    dVar4 = (double)_FindSourceOffset(param_1);
    puVar1[0xc] = (int)((double)_DAT_1000d660 * dVar4);
    puVar1[0xe] = (int)((double)_DAT_1000d660 * (param_1 - param_3) + param_2);
    puVar1[0xd] = *(undefined4 *)(local_6c + 0x17b4);
    uVar2 = _GrainToDisk(puVar1);
    .glue::DisposePtr(puVar1);
  }
  return uVar2;
}




/* Função: .VerifyQuit | Endereço: 10001e68 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

bool .debug::_VerifyQuit(void)

{
  int iVar1;
  undefined4 uVar2;
  char cVar3;
  undefined4 local_48;
  undefined4 local_44;
  undefined1 auStack_40 [4];
  undefined1 auStack_3c [4];
  char local_38;
  char local_37;
  undefined4 local_36;
  undefined4 local_32;
  undefined4 local_2e;
  undefined4 local_2a;
  
  local_37 = '\0';
  local_38 = '\0';
  iVar1 = .glue::GetNewDialog(0x88,0,0xffffffff);
  .glue::GetPort(&local_48);
  .glue::SetPort(iVar1);
  .glue::ShowWindow(iVar1);
  .glue::DrawDialog(iVar1);
  .glue::DrawControls(iVar1);
  .glue::GetDialogItem(iVar1,1,auStack_40,auStack_3c,&local_2e);
  .glue::GetDialogItem(iVar1,2,auStack_40,auStack_3c,&local_36);
  do {
    .glue::GetMouse(&local_44);
    uVar2 = .glue::PinRect(&local_2e,local_44);
    if (((local_44._0_2_ == (short)((uint)uVar2 >> 0x10)) && (local_44._2_2_ == (short)uVar2)) &&
       (cVar3 = .glue::Button(), cVar3 != '\0')) {
      _DoCustomButton(_DAT_1000d61c,_DAT_1000d620,local_2e,local_2a,&local_37);
    }
    else {
      uVar2 = .glue::PinRect(&local_36,local_44);
      if (((local_44._0_2_ == (short)((uint)uVar2 >> 0x10)) && (local_44._2_2_ == (short)uVar2)) &&
         (cVar3 = .glue::Button(), cVar3 != '\0')) {
        _DoCustomButton(_DAT_1000d614,_DAT_1000d618,local_36,local_32,&local_38);
      }
    }
  } while ((local_37 == '\0') && (local_38 == '\0'));
  .glue::FlushEvents(0xffff,0);
  if (iVar1 != 0) {
    .glue::DisposeDialog(iVar1);
  }
  .glue::SetPort(local_48);
  return local_38 == '\0';
}




/* Função: .GetConsole | Endereço: 100020a8 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_GetConsole(void)

{
  int local_4c;
  undefined1 auStack_28 [16];
  undefined1 auStack_18 [4];
  undefined1 auStack_14 [4];
  undefined2 local_10 [6];
  
  local_10[0] = 0;
  _DAT_1000d65c = .glue::GetNewDialog(0x7d1,0,0xffffffff);
  .glue::SetWTitle(_DAT_1000d65c,local_4c + 0x164c);
  .glue::GetNextEvent(0xffff,auStack_28);
  .glue::DialogSelect(auStack_28,&DAT_1000d65c,local_10);
  .glue::GetNextEvent(0xffff,auStack_28);
  .glue::DialogSelect(auStack_28,&DAT_1000d65c,local_10);
  .glue::GetDialogItem(_DAT_1000d65c,2,auStack_18,auStack_14,local_4c + 0x1542);
  .glue::GetDialogItem(_DAT_1000d65c,3,auStack_18,auStack_14,local_4c + 0x153a);
  *(undefined2 *)(_DAT_1000d65c + 0x44) = 3;
  *(undefined2 *)(_DAT_1000d65c + 0x4a) = 9;
  return;
}




/* Função: .UpdateConsole | Endereço: 100021bc */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_UpdateConsole(double param_1,double param_2,double param_3)

{
  int *piVar1;
  undefined4 uVar2;
  short sVar3;
  char cVar4;
  undefined4 uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int in_r7;
  int iVar9;
  uint uVar10;
  double dVar11;
  double dVar12;
  int local_4dc;
  undefined1 auStack_4b8 [2];
  char local_4b6;
  undefined1 auStack_3b8 [256];
  char local_2b8 [4];
  undefined4 local_2b4 [2];
  ushort local_2ac;
  uint local_2aa;
  undefined4 local_2a2 [69];
  undefined4 local_18e;
  undefined4 local_18a;
  undefined1 local_178 [256];
  undefined8 local_78;
  undefined8 local_70;
  undefined8 local_68;
  undefined8 local_60;
  
  piVar1 = (int *).glue::GetGrayRgn();
  .glue::SetPort(_DAT_1000d65c);
  if (*(double *)(local_4dc + 0x498) != param_1) {
    .glue::MoveTo(100,0x43);
    .glue::DrawString(local_4dc + 0x164c);
    .glue::MoveTo(0x96,0x5c);
    .glue::DrawString(&DAT_1000c67b);
    .glue::MoveTo(0x96,0x70);
    .glue::DrawString(&DAT_1000c67b);
    .glue::MoveTo(0x96,0x84);
    .glue::DrawString(&DAT_1000c685);
    .glue::MoveTo(0x96,0x98);
    .glue::DrawString(s_samples_semitones_1000c686 + 8);
    .glue::DrawPicture(*(undefined4 *)(local_4dc + 0x1780),local_4dc + 0x153a);
    if ((((*(double *)(local_4dc + 0x468) != param_1) || (*(double *)(local_4dc + 0x460) != param_2)
         ) || (*(int *)(local_4dc + 0x458) != in_r7)) || (*(double *)(local_4dc + 0x450) != param_3)
       ) {
      local_178[0] = 0;
      iVar6 = (int)(*(double *)(local_4dc + 0x490) * param_1);
      local_60 = (double)(longlong)iVar6;
      uVar10 = (uint)(short)iVar6;
      local_68 = (double)CONCAT44(0x43300000,uVar10 * 0x3c ^ 0x80000000);
      local_70 = (double)(longlong)(int)(param_1 - (local_68 - *(double *)(local_4dc + 0x470)));
      uVar2 = .glue::NumToString(uVar10,auStack_3b8);
      local_78 = (double)CONCAT44(0x43300000,uVar10 ^ 0x80000000);
      _FltToStr((double)(float)-(*(double *)(local_4dc + 0x488) *
                                 (double)(float)(local_78 - *(double *)(local_4dc + 0x470)) -
                                param_1),uVar2,auStack_4b8);
      _pstrcat(local_178,auStack_3b8);
      _pstrcat(local_178,local_4dc + 0x448);
      if (local_4b6 == '.') {
        _pstrcat(local_178,local_4dc + 0x44a);
      }
      _pstrcat(local_178,auStack_4b8);
      .glue::MoveTo(100,0x5c);
      .glue::DrawString(local_178);
      local_178[0] = 0;
      *(double *)(local_4dc + 0x468) = param_1;
      iVar6 = (int)(*(double *)(local_4dc + 0x490) * param_2);
      local_78 = (double)(longlong)iVar6;
      uVar10 = (uint)(short)iVar6;
      local_70 = (double)CONCAT44(0x43300000,uVar10 * 0x3c ^ 0x80000000);
      local_68 = (double)(longlong)(int)(param_2 - (local_70 - *(double *)(local_4dc + 0x470)));
      uVar2 = .glue::NumToString(uVar10,auStack_3b8);
      local_60 = (double)CONCAT44(0x43300000,uVar10 ^ 0x80000000);
      _FltToStr((double)(float)-(*(double *)(local_4dc + 0x488) *
                                 (double)(float)(local_60 - *(double *)(local_4dc + 0x470)) -
                                param_2),uVar2,auStack_4b8);
      _pstrcat(local_178,auStack_3b8);
      _pstrcat(local_178,local_4dc + 0x448);
      if (local_4b6 == '.') {
        _pstrcat(local_178,local_4dc + 0x44a);
      }
      _pstrcat(local_178,auStack_4b8);
      .glue::MoveTo(100,0x70);
      .glue::DrawString(local_178);
      *(double *)(local_4dc + 0x460) = param_2;
      .glue::NumToString(in_r7,local_178);
      .glue::MoveTo(100,0x84);
      .glue::DrawString(local_178);
      *(int *)(local_4dc + 0x458) = in_r7;
      dVar11 = .glue::log(*(double *)(local_4dc + 0x478));
      dVar12 = .glue::log(param_3);
      _FltToStr((double)(float)(*(double *)(local_4dc + 0x480) * (dVar12 / dVar11)));
      .glue::MoveTo(100,0x98);
      .glue::DrawString(local_178);
      *(double *)(local_4dc + 0x450) = param_3;
    }
  }
  .glue::SystemTask();
  .glue::WaitNextEvent(0xffff,&local_2ac,0,0);
  if (local_2ac == 6) {
    .glue::BeginUpdate(_DAT_1000d65c);
    .glue::DrawDialog(_DAT_1000d65c);
    .glue::MoveTo(0x96,0x5c);
    .glue::DrawString(&DAT_1000c67b);
    .glue::MoveTo(0x96,0x70);
    .glue::DrawString(&DAT_1000c67b);
    .glue::MoveTo(0x96,0x84);
    .glue::DrawString(&DAT_1000c685);
    .glue::MoveTo(0x96,0x98);
    .glue::DrawString(s_samples_semitones_1000c686 + 8);
    .glue::EndUpdate(_DAT_1000d65c);
  }
  else if (local_2ac < 6) {
    if (local_2ac == 1) {
      sVar3 = .glue::FindWindow(local_2a2[0],local_2b4);
      if (sVar3 != 2) {
        if (1 < sVar3) {
          if (sVar3 == 4) {
            uVar5 = *(undefined4 *)(*piVar1 + 2);
            uVar2 = *(undefined4 *)(*piVar1 + 6);
            local_18e._2_2_ = (short)uVar5;
            iVar6 = (int)local_18e._2_2_;
            local_18e._0_2_ = (short)((uint)uVar5 >> 0x10);
            iVar7 = (int)local_18e._0_2_;
            local_18a._2_2_ = (short)uVar2;
            iVar8 = (int)local_18a._2_2_;
            local_18a._0_2_ = (short)((uint)uVar2 >> 0x10);
            iVar9 = (int)local_18a._0_2_;
            local_18e = uVar5;
            local_18a = uVar2;
            .glue::SetRect(&local_18e,iVar6 + 2,iVar7 + 0x19,iVar8 + -2,iVar9 + -2);
            .glue::DragWindow(_DAT_1000d65c,local_2a2[0],&local_18e);
            return;
          }
          if (3 < sVar3) {
            return;
          }
          .glue::SysBeep(3);
          .glue::GlobalToLocal(local_2a2);
          cVar4 = .glue::PtInRect(local_2a2[0],&DAT_1000d402);
          if (cVar4 == '\0') {
            return;
          }
          _DoCustomButton(*(undefined4 *)(local_4dc + 0x1788),*(undefined4 *)(local_4dc + 0x1784),
                          _DAT_1000d402,uRam1000d406,local_2b8);
          if (local_2b8[0] == '\0') {
            return;
          }
          cVar4 = _VerifyQuit();
          if (cVar4 == '\0') {
            return;
          }
          DAT_1000d670 = 1;
          return;
        }
        if (sVar3 != 0) {
          if (sVar3 < 0) {
            return;
          }
          .glue::MenuSelect(local_2a2[0]);
          cVar4 = _HandleThonkMenu();
          if (cVar4 == '\0') {
            return;
          }
          cVar4 = _VerifyQuit();
          if (cVar4 == '\0') {
            return;
          }
          DAT_1000d670 = 1;
          return;
        }
      }
      .glue::SystemClick(&local_2ac,local_2b4[0]);
    }
  }
  else if ((local_2ac == 0xf) && (local_2aa >> 0x18 == 1)) {
    if ((local_2aa & 1) == 0) {
      DAT_1000d64d = 1;
      _DAT_1000d3f8 = _DAT_1000d658;
      _DAT_1000d658 = 0;
    }
    else {
      DAT_1000d64d = 0;
      _DAT_1000d658 = _DAT_1000d3f8;
    }
  }
  return;
}




/* Função: .MakeCosineAttack | Endereço: 1000285c */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_MakeCosineAttack(void)

{
  uint uVar1;
  double dVar2;
  double dVar3;
  double dVar4;
  double dVar5;
  undefined4 local_5c;
  undefined8 local_30;
  
  dVar3 = _DAT_1000c380;
  uVar1 = .glue::GetHandleSize(_DAT_1000d6c0);
  local_30 = (double)CONCAT44(0x43300000,uVar1 ^ 0x80000000);
  local_30 = local_30 - *(double *)(local_5c + 0x4b0);
  dVar4 = *(double *)(local_5c + 0x4a8);
  dVar5 = *(double *)(local_5c + 0x4b8);
  for (; dVar3 < local_30; dVar3 = dVar3 + dVar5) {
    dVar2 = .glue::cos((dVar3 / local_30) * _DAT_1000c6b0);
    _IntToHandle((int)-(dVar4 * dVar2 - dVar4),(int)dVar3,_DAT_1000d6c0);
  }
  return;
}




/* Função: .MakeCosineDecay | Endereço: 10002960 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_MakeCosineDecay(void)

{
  uint uVar1;
  double dVar2;
  double dVar3;
  double dVar4;
  double dVar5;
  double dVar6;
  undefined4 local_6c;
  undefined8 local_40;
  
  dVar3 = _DAT_1000c380;
  uVar1 = .glue::GetHandleSize(_DAT_1000d6bc);
  local_40 = (double)CONCAT44(0x43300000,uVar1 ^ 0x80000000);
  local_40 = local_40 - *(double *)(local_6c + 0x4b0);
  dVar4 = *(double *)(local_6c + 0x4a0);
  dVar5 = *(double *)(local_6c + 0x4a8);
  dVar6 = *(double *)(local_6c + 0x4b8);
  for (; dVar3 < local_40; dVar3 = dVar3 + dVar6) {
    dVar2 = .glue::cos((dVar3 / local_40) * _DAT_1000c6b0);
    _IntToHandle((int)(dVar4 * dVar2 + dVar5),(int)dVar3,_DAT_1000d6bc);
  }
  return;
}




/* Função: .FindGrainlength | Endereço: 10002a70 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int .debug::_FindGrainlength(undefined8 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 extraout_r4;
  double dVar1;
  double dVar2;
  double dVar3;
  
  dVar1 = (double)_FindLinParm(param_1,&DAT_1000d6b8,param_3,_DAT_1000d6b8);
  dVar2 = (double)_FindLinParm(param_1,&DAT_1000d698,extraout_r4,_DAT_1000d698);
  dVar3 = (double)_FRand();
  return (int)(dVar1 + (dVar2 * dVar3 - _DAT_1000c3a8 * dVar2));
}




/* Função: .FindAttack | Endereço: 10002b18 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

double .debug::_FindAttack(undefined8 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 extraout_r4;
  double dVar1;
  double dVar2;
  double dVar3;
  
  dVar1 = (double)_FindLinParm(param_1,&DAT_1000d6b4,param_3,_DAT_1000d6b4);
  dVar2 = (double)_FindLinParm(param_1,&DAT_1000d694,extraout_r4,_DAT_1000d694);
  dVar3 = (double)_FRand();
  return dVar1 + (dVar2 * dVar3 - _DAT_1000c3a8 * dVar2);
}




/* Função: .FindDecay | Endereço: 10002bb4 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

double .debug::_FindDecay(undefined8 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 extraout_r4;
  double dVar1;
  double dVar2;
  double dVar3;
  
  dVar1 = (double)_FindLinParm(param_1,&DAT_1000d6b0,param_3,_DAT_1000d6b0);
  dVar2 = (double)_FindLinParm(param_1,&DAT_1000d690,extraout_r4,_DAT_1000d690);
  dVar3 = (double)_FRand();
  return dVar1 + (dVar2 * dVar3 - _DAT_1000c3a8 * dVar2);
}




/* Função: .FindGain | Endereço: 10002c4c */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

double .debug::_FindGain(undefined8 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 extraout_r4;
  double dVar1;
  double dVar2;
  double dVar3;
  
  dVar1 = (double)_FindLinParm(param_1,&DAT_1000d6ac,param_3,_DAT_1000d6ac);
  dVar2 = (double)_FindLinParm(param_1,&DAT_1000d688,extraout_r4,_DAT_1000d688);
  dVar3 = (double)_FRand();
  return dVar1 + (dVar2 * dVar3 - _DAT_1000c3a8 * dVar2);
}




/* Função: .FindSourceOffset | Endereço: 10002ce0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

double .debug::_FindSourceOffset(undefined8 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 extraout_r4;
  double dVar1;
  double dVar2;
  double dVar3;
  
  dVar1 = (double)_FindLinParm(param_1,&DAT_1000d6a8,param_3,_DAT_1000d6a8);
  dVar2 = (double)_FindLinParm(param_1,&DAT_1000d684,extraout_r4,_DAT_1000d684);
  dVar3 = (double)_FRand();
  return dVar1 + (dVar2 * dVar3 - _DAT_1000c3a8 * dVar2);
}




/* Função: .FindBalance | Endereço: 10002d7c */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

double .debug::_FindBalance(undefined8 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 extraout_r4;
  double dVar1;
  double dVar2;
  double dVar3;
  
  dVar1 = (double)_FindLinParm(param_1,&DAT_1000d6a0,param_3,_DAT_1000d6a0);
  dVar2 = (double)_FindLinParm(param_1,&DAT_1000d67c,extraout_r4,_DAT_1000d67c);
  dVar3 = (double)_FRand();
  return dVar1 + (dVar2 * dVar3 - _DAT_1000c3a8 * dVar2);
}




/* Função: .FindGranFreq | Endereço: 10002e14 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_FindGranFreq(undefined4 param_1,undefined4 param_2)

{
  _FindExpParm(&DAT_1000d6a4,param_2,_DAT_1000d6a4);
  return;
}




/* Função: .FindFrequencyMultiplier | Endereço: 10002e60 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

double .debug::_FindFrequencyMultiplier(double param_1)

{
  short sVar1;
  double *pdVar2;
  double *pdVar3;
  short sVar4;
  short sVar5;
  short sVar6;
  short unaff_r31;
  double dVar7;
  double dVar8;
  double dVar9;
  double dVar10;
  double dVar11;
  undefined8 local_68;
  undefined8 local_60;
  short sStack_52;
  undefined8 local_50;
  
  if (*_DAT_1000d69c <= param_1) {
    pdVar2 = (double *)_GetInsertFMBP(param_1);
    pdVar3 = *(double **)(pdVar2 + 9);
    if (pdVar3 == (double *)0x0) {
      dVar11 = pdVar2[1];
      dVar10 = pdVar2[2];
      dVar9 = pdVar2[3];
      dVar8 = pdVar2[4];
      sVar6 = (short)(int)pdVar2[5];
      sVar1 = (short)(int)pdVar2[6];
      sStack_52 = (short)(int)pdVar2[7];
    }
    else {
      dVar7 = *pdVar3 - *pdVar2;
      if (*pdVar3 - *pdVar2 == _DAT_1000c3a0) {
        dVar7 = _DAT_1000c6a0;
      }
      dVar7 = (param_1 - *pdVar2) / dVar7;
      dVar11 = dVar7 * (pdVar3[1] - pdVar2[1]) + pdVar2[1];
      dVar10 = dVar7 * (pdVar3[2] - pdVar2[2]) + pdVar2[2];
      dVar9 = dVar7 * (pdVar3[3] - pdVar2[3]) + pdVar2[3];
      local_60 = (double)CONCAT44(0x43300000,(int)(short)(int)(pdVar3[5] - pdVar2[5]) ^ 0x80000000);
      dVar8 = dVar7 * (pdVar3[4] - pdVar2[4]) + pdVar2[4];
      local_50 = (double)CONCAT44(0x43300000,(int)(short)(int)(pdVar3[6] - pdVar2[6]) ^ 0x80000000);
      local_68 = (double)CONCAT44(0x43300000,(int)(short)(int)(pdVar3[7] - pdVar2[7]) ^ 0x80000000);
      sVar6 = (short)(int)(dVar7 * (local_60 - _DAT_1000c388) + pdVar2[5]);
      sVar1 = (short)(int)(dVar7 * (local_50 - _DAT_1000c388) + pdVar2[6]);
      sStack_52 = (short)(int)(dVar7 * (local_68 - _DAT_1000c388) + pdVar2[7]);
    }
  }
  else {
    dVar11 = _DAT_1000d69c[1];
    dVar10 = _DAT_1000d69c[2];
    dVar9 = _DAT_1000d69c[3];
    dVar8 = _DAT_1000d69c[4];
    sVar6 = (short)(int)_DAT_1000d69c[5];
    sVar1 = (short)(int)_DAT_1000d69c[6];
    sStack_52 = (short)(int)_DAT_1000d69c[7];
  }
  sVar5 = sVar6 + sVar1 + sStack_52;
  dVar7 = (double)_FRand();
  sVar4 = (short)(int)(_DAT_1000c398 * dVar7);
  if (sVar4 < sVar6) {
    unaff_r31 = 1;
  }
  else if (sVar4 < (short)(sVar6 + sVar1)) {
    unaff_r31 = 2;
  }
  if (((short)(sVar6 + sVar1) <= sVar4) && (sVar4 < sVar5)) {
    unaff_r31 = 3;
  }
  if (sVar5 <= sVar4) {
    unaff_r31 = 4;
  }
  if (unaff_r31 != 3) {
    if (unaff_r31 < 3) {
      dVar9 = dVar10;
      if (1 < unaff_r31) goto LAB_100031c4;
    }
    else {
      dVar9 = dVar8;
      if (unaff_r31 < 5) goto LAB_100031c4;
    }
    dVar9 = dVar11;
  }
LAB_100031c4:
  dVar8 = (double)_FindLinParm(param_1,&DAT_1000d68c,(int)(_DAT_1000c398 * dVar7),_DAT_1000d68c);
  dVar10 = (double)_FRand();
  dVar9 = .glue::pow(_DAT_1000c390,dVar9 + (dVar8 * dVar10 - _DAT_1000c3a8 * dVar8));
  return dVar9;
}




/* Função: .FltToStr | Endereço: 10003268 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_FltToStr(double param_1,undefined4 param_2,byte *param_3)

{
  double dVar1;
  double dVar2;
  double dVar3;
  int iVar4;
  short sVar5;
  short sVar6;
  double dVar7;
  double dVar8;
  double dVar9;
  double dVar10;
  double dVar11;
  double dVar12;
  double dVar13;
  
  *param_3 = 0;
  sVar5 = 1;
  if (param_1 < (double)_DAT_1000c3ec) {
    param_1 = -param_1;
    *param_3 = *param_3 + 1;
    sVar5 = 2;
    param_3[*param_3] = 0x2d;
  }
  dVar9 = _DAT_1000c3d0;
  dVar12 = _DAT_1000c3c8;
  dVar3 = _DAT_1000c3c0;
  dVar2 = _DAT_1000c3b8;
  dVar1 = _DAT_1000c3b0;
  if ((double)_DAT_1000c3e8 <= param_1) {
    dVar7 = (double)(float)(_DAT_1000c3c8 + param_1);
    for (dVar13 = _DAT_1000c3c0; dVar13 < dVar7; dVar13 = dVar13 * _DAT_1000c3d0) {
    }
    dVar13 = dVar13 / _DAT_1000c3d0;
    dVar10 = (double)_DAT_1000c3d8;
    while (sVar6 = sVar5, dVar12 < dVar13) {
      sVar5 = sVar6 + 1;
      *param_3 = *param_3 + 1;
      dVar8 = (double)(float)((double)CONCAT44(0x43300000,
                                               (int)(dVar7 / (double)(float)dVar13) ^ 0x80000000) -
                             dVar1);
      dVar11 = dVar8 * dVar13;
      dVar13 = dVar13 / dVar9;
      dVar7 = (double)(float)(dVar7 - dVar11);
      param_3[sVar6] = (byte)(int)(dVar10 + dVar8);
      if ((dVar13 < dVar3) && (dVar2 < dVar13)) {
        *param_3 = *param_3 + 1;
        param_3[sVar5] = 0x2e;
        sVar5 = sVar6 + 2;
      }
    }
  }
  else {
    *param_3 = *param_3 + 2;
    sVar5 = 3;
    param_3[1] = 0x30;
    param_3[2] = 0x2e;
    dVar3 = _DAT_1000c3d0;
    dVar2 = _DAT_1000c3c8;
    dVar1 = _DAT_1000c3b0;
    dVar9 = (double)_DAT_1000c3d8;
    dVar12 = _DAT_1000c3e0;
    while (dVar2 < dVar12) {
      iVar4 = (int)sVar5;
      sVar5 = sVar5 + 1;
      *param_3 = *param_3 + 1;
      dVar7 = (double)(float)((double)CONCAT44(0x43300000,(int)(param_1 / dVar12) ^ 0x80000000) -
                             dVar1);
      dVar13 = dVar7 * dVar12;
      dVar12 = dVar12 / dVar3;
      param_1 = (double)(float)-(dVar13 - param_1);
      param_3[iVar4] = (byte)(int)(dVar9 + dVar7);
    }
  }
  return;
}




/* Função: .main | Endereço: 10003468 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_main(void)

{
  undefined4 uVar1;
  short sVar2;
  undefined4 local_7c;
  
  _InitAIFFTools();
  uVar1 = .glue::GetResource(0x54686e4b,0);
  .glue::ResError();
  sVar2 = .glue::HomeResFile(uVar1);
  .glue::ReleaseResource(uVar1);
  _DAT_1000d40a = sVar2;
  .glue::UseResFile((int)sVar2);
  _DAT_1000d63c = .glue::GetPicture(0x3ed);
  _DAT_1000d638 = .glue::GetPicture(0x3ec);
  _DAT_1000d634 = .glue::GetPicture(0x3eb);
  _DAT_1000d630 = .glue::GetPicture(0x3ea);
  _DAT_1000d62c = .glue::GetPicture(0x3e9);
  _DAT_1000d628 = .glue::GetPicture(1000);
  _DAT_1000d624 = .glue::GetPicture(0x80);
  _DAT_1000d620 = .glue::GetPicture(0x452);
  _DAT_1000d61c = .glue::GetPicture(0x453);
  _DAT_1000d618 = .glue::GetPicture(0x454);
  _DAT_1000d614 = .glue::GetPicture(0x455);
  _DAT_1000d648 = .glue::GetPicture(0x44f);
  _DAT_1000d644 = .glue::GetPicture(0x44e);
  _DAT_1000d640 = .glue::GetPicture(0x82);
  _DAT_1000d610 = .glue::GetPicture(0x456);
  uVar1 = .glue::GetPicture(0x457);
  *(undefined4 *)(local_7c + 0x174c) = uVar1;
  _SetupMenuBar();
  do {
    do {
      sVar2 = _GoGoGranes();
      .glue::DisposeHandle(_DAT_1000d6c4);
    } while (sVar2 == -0x24);
  } while (sVar2 == -0x25);
  if (_DAT_1000d624 != 0) {
    .glue::DisposeHandle();
  }
  if (_DAT_1000d63c != 0) {
    .glue::DisposeHandle();
  }
  if (_DAT_1000d638 != 0) {
    .glue::DisposeHandle();
  }
  if (_DAT_1000d634 != 0) {
    .glue::DisposeHandle();
  }
  if (_DAT_1000d630 != 0) {
    .glue::DisposeHandle();
  }
  if (_DAT_1000d62c != 0) {
    .glue::DisposeHandle();
  }
  if (_DAT_1000d628 != 0) {
    .glue::DisposeHandle();
  }
  if (_DAT_1000d620 != 0) {
    .glue::DisposeHandle();
  }
  if (_DAT_1000d61c != 0) {
    .glue::DisposeHandle();
  }
  if (_DAT_1000d618 != 0) {
    .glue::DisposeHandle();
  }
  if (_DAT_1000d614 != 0) {
    .glue::DisposeHandle();
  }
  if (_DAT_1000d648 != 0) {
    .glue::DisposeHandle();
  }
  if (_DAT_1000d644 != 0) {
    .glue::DisposeHandle();
  }
  if (_DAT_1000d640 != 0) {
    .glue::DisposeHandle();
  }
  if (_DAT_1000d610 != 0) {
    .glue::DisposeHandle();
  }
  if (*(int *)(local_7c + 0x174c) != 0) {
    .glue::DisposeHandle();
  }
  return;
}




/* Função: .SetupMenuBar | Endereço: 100037c4 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_SetupMenuBar(void)

{
  undefined4 uVar1;
  
  _DAT_1000d6c4 = .glue::GetNewMBar(0x80);
  .glue::SetMenuBar(_DAT_1000d6c4);
  uVar1 = .glue::GetMenuHandle(0x80);
  .glue::AppendResMenu(uVar1,0x44525652);
  .glue::DrawMenuBar();
  return;
}




/* Função: .Do16BitMonoWarning | Endereço: 1000384c */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_Do16BitMonoWarning(void)

{
  undefined4 uVar1;
  undefined4 uVar2;
  char cVar3;
  undefined4 local_38;
  undefined4 local_34;
  undefined1 auStack_30 [4];
  undefined1 auStack_2c [4];
  char local_28 [2];
  undefined4 local_26;
  undefined4 local_22;
  
  local_28[0] = '\0';
  uVar1 = .glue::GetNewDialog(0xd2,0,0xffffffff);
  .glue::GetPort(&local_38);
  .glue::SetPort(uVar1);
  .glue::ShowWindow(uVar1);
  .glue::DrawDialog(uVar1);
  .glue::DrawControls(uVar1);
  .glue::GetDialogItem(uVar1,2,auStack_30,auStack_2c,&local_26);
  while (local_28[0] == '\0') {
    .glue::GetMouse(&local_34);
    uVar2 = .glue::PinRect(&local_26,local_34);
    if (local_34._0_2_ == (short)((uint)uVar2 >> 0x10)) {
      if ((local_34._2_2_ == (short)uVar2) && (cVar3 = .glue::Button(), cVar3 != '\0')) {
        _DoCustomButton(_DAT_1000d60c,_DAT_1000d610,local_26,local_22,local_28);
      }
    }
  }
  .glue::DisposeDialog(uVar1);
  .glue::SetPort(local_38);
  return;
}




/* Função: .GoGoGranes | Endereço: 100039cc */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 .debug::_GoGoGranes(void)

{
  uint uVar1;
  undefined **ppuVar2;
  char cVar4;
  short sVar3;
  undefined4 extraout_r4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined **local_7c;
  undefined1 auStack_58 [40];
  undefined4 local_30;
  uint uStack_2c;
  
  uVar6 = 0;
  uVar5 = 0;
  cVar4 = _TrapAvailable(0xffffa1ad);
  ppuVar2 = &.TVect::InitGraf;
  if ((cVar4 != '\0') && (cVar4 = _HasFPU(), cVar4 == '\0')) {
    uVar6 = 0xffffb1e0;
    .glue::Alert(0x8d,0);
    ppuVar2 = local_7c;
  }
  if (((short)uVar6 == 0) && (uVar6 = _ThonkDialog(), (short)uVar6 == 0)) {
    do {
      uVar6 = _OpenAIFFSourceFile(&DAT_1000d67a);
      if (((short)uVar6 == 0) &&
         ((_GetAIFFInfo((int)_DAT_1000d67a,&DAT_1000d660), sRam1000d66c != 1 ||
          (sRam1000d664 != 0x10)))) {
        _Do16BitMonoWarning();
        _CloseAIFFSourceFile((int)_DAT_1000d67a);
        uVar6 = 0xffffb1e0;
      }
    } while ((short)uVar6 == -20000);
    if ((short)uVar6 == 0) {
      if ((uint)(int)_DAT_1000c6aa < 0x12) {
                    /* WARNING: Could not recover jumptable at 0x10003aec. Too many branches */
                    /* WARNING: Treating indirect jump as call */
        uVar5 = (*(code *)ppuVar2[(int)_DAT_1000c6aa + 0x230])();
        return uVar5;
      }
      .glue::c2pstr(auStack_58);
      if (0xc < DAT_1000d40c) {
        DAT_1000d40c = 0xc;
      }
      _pstrcat(&DAT_1000d40c,local_7c + 0x14d);
      _pstrcat(&DAT_1000d40c,auStack_58);
      uVar6 = _NewAIFFDestinationFile
                        ((double)_DAT_1000d660,2,extraout_r4,&DAT_1000d40c,0x53643261,&DAT_1000d678)
      ;
      if ((short)uVar6 == 0) {
        _StartGrainDamage();
        _SetFirstBreakPoints();
        _FindBeginOfSoundData((int)_DAT_1000d67a);
        .glue::GetFPos((int)_DAT_1000d67a,local_7c + 0x5ed);
        uStack_2c = uRam1000d668 ^ 0x80000000;
        local_30 = 0x43300000;
        *(double *)(local_7c + 0x5e4) =
             (double)((float)((double)CONCAT44(0x43300000,uStack_2c) - *(double *)(local_7c + 0x14e)
                             ) / _DAT_1000d660);
        _GetConsole();
        _SetDefaultValues();
        .glue::GetDateTime(local_7c + 0x6f7);
        sVar3 = _DAT_1000c6aa;
        while ((((uVar1 = (uint)sVar3, uVar1 == 3 || (uVar1 == 7)) || (uVar1 == 10)) ||
               ((uVar1 == 0xd || (uVar1 == 0x11))))) {
          sVar3 = .glue::Random();
          uVar1 = (uint)sVar3;
          sVar3 = FUN_1000ada4(uVar1 + (((int)uVar1 >> 4) +
                                       (uint)((int)uVar1 < 0 && (uVar1 & 0xf) != 0)) * -0x10);
        }
        if (uVar1 < 0x10) {
                    /* WARNING: Could not recover jumptable at 0x10003d50. Too many branches */
                    /* WARNING: Treating indirect jump as call */
          uVar5 = (*(code *)local_7c[uVar1 + 0x220])();
          return uVar5;
        }
        _CloseAIFFDestinationFile((int)_DAT_1000d678);
        _StopGrainDamage();
      }
      if ((short)uVar6 != 0) {
        uVar5 = uVar6;
      }
      uVar6 = _CloseAIFFSourceFile((int)_DAT_1000d67a);
    }
  }
  if ((short)uVar5 != 0) {
    uVar6 = uVar5;
  }
  return uVar6;
}




/* Função: .HasFPU | Endereço: 10003e50 */

undefined4 .debug::_HasFPU(void)

{
  return 1;
}




/* Função: .TrapAvailable | Endereço: 10003e74 */

bool .debug::_TrapAvailable(uint param_1)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  ushort uVar4;
  
  bVar1 = (param_1 & 0x800) != 0;
  iVar2 = .glue::NGetTrapAddress(0xaa6e,1);
  iVar3 = .glue::NGetTrapAddress(0xa86e,1);
  if (iVar3 == iVar2) {
    uVar4 = 0x200;
  }
  else {
    uVar4 = 0x400;
  }
  if ((bVar1) && (param_1 = param_1 & 0x7ff, uVar4 <= (ushort)param_1)) {
    param_1 = 0xffffa89f;
  }
  iVar2 = .glue::NGetTrapAddress(0xa89f,1);
  iVar3 = .glue::NGetTrapAddress(param_1,bVar1);
  return iVar3 != iVar2;
}




/* Função: .pstrcat | Endereço: 10003f84 */

void .debug::_pstrcat(byte *param_1,char *param_2)

{
  .glue::BlockMove(param_2 + 1,param_1 + *param_1 + 1,*param_2);
  *param_1 = *param_1 + *param_2;
  return;
}




/* Função: .GrainToDisk | Endereço: 10004000 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 .debug::_GrainToDisk(uint *param_1)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined4 unaff_r20;
  uint uVar8;
  int iVar9;
  double dVar10;
  undefined8 uVar11;
  undefined8 uVar12;
  undefined8 uVar13;
  double dVar14;
  double dVar15;
  double dVar16;
  int local_bc;
  undefined1 auStack_88 [8];
  undefined8 local_80;
  undefined4 local_78;
  uint uStack_74;
  longlong local_70;
  undefined4 local_68;
  uint uStack_64;
  
  iVar2 = _BorderLong(*param_1,0,0x6baa8);
  uVar11 = _BorderDouble(*(undefined8 *)(param_1 + 1),_DAT_1000c438,_DAT_1000c430);
  uVar12 = _BorderDouble(*(undefined8 *)(param_1 + 3),_DAT_1000c438,_DAT_1000c430);
  uVar13 = _BorderDouble(*(undefined8 *)(param_1 + 5),_DAT_1000c438,_DAT_1000c428);
  dVar14 = (double)_BorderDouble(*(undefined8 *)(param_1 + 7),_DAT_1000c420,_DAT_1000c428);
  uStack_64 = *param_1 ^ 0x80000000;
  local_68 = 0x43300000;
  iVar5 = (int)(((double)CONCAT44(0x43300000,uStack_64) - _DAT_1000c408) * dVar14);
  local_70 = (longlong)iVar5;
  dVar15 = (double)_BorderDouble(*(undefined8 *)(param_1 + 9),_DAT_1000c438,_DAT_1000c418);
  _DAT_1000d67a = *(undefined2 *)(param_1 + 0xb);
  uVar3 = _BorderLong(param_1[0xc],0,iRam1000d668 - iVar5);
  uVar4 = _BorderLong(param_1[0xe],0,0x7fffffff);
  uVar8 = uVar3 ^ 0x80000000;
  local_78 = 0x43300000;
  if (iVar2 != 0) {
    uStack_74 = uVar8;
    iVar5 = .glue::NewPtr(iVar2 << 1);
    unaff_r20 = .glue::MemError();
    if ((short)unaff_r20 == 0) {
      unaff_r20 = _GetGrain(dVar14,iVar2);
      if ((short)unaff_r20 == 0) {
        _ShapeGrain2(uVar11,uVar12,uVar13,iVar2);
        if (*(char *)(local_bc + 0x7e8) == '\0') {
          unaff_r20 = _AIFFSndAdd((int)_DAT_1000d678,iVar5,iVar2,1,uVar4,auStack_88);
          if ((short)unaff_r20 == 0) {
            if (_DAT_1000d6c8 == 0) {
              local_80 = (double)CONCAT44(0x43300000,uVar4 ^ 0x80000000);
              local_78 = 0x43300000;
              uStack_74 = uVar8;
              _UpdateConsole((double)((float)(local_80 - *(double *)(local_bc + 0x548)) /
                                     _DAT_1000d660),
                             (double)((float)((double)CONCAT44(0x43300000,uVar8) -
                                             *(double *)(local_bc + 0x548)) / _DAT_1000d660),dVar14)
              ;
              _DAT_1000d6c8 = _DAT_1000d658;
            }
            else {
              _DAT_1000d6c8 = _DAT_1000d6c8 + -1;
            }
          }
        }
        else {
          iVar6 = .glue::NewPtr(iVar2 << 1);
          unaff_r20 = .glue::MemError();
          if ((short)unaff_r20 == 0) {
            dVar10 = *(double *)(local_bc + 0x570);
            dVar15 = dVar15 * *(double *)(local_bc + 0x550);
            dVar16 = (double).glue::sqrt(dVar15);
            dVar15 = (double).glue::sqrt(dVar10 - dVar15);
            iVar7 = 0;
            dVar10 = *(double *)(local_bc + 0x548);
            iVar9 = iVar2;
            if (0 < iVar2) {
              do {
                uStack_74 = (int)*(short *)(iVar5 + iVar7) ^ 0x80000000;
                local_78 = 0x43300000;
                iVar1 = (int)(dVar16 * ((double)CONCAT44(0x43300000,uStack_74) - dVar10));
                local_70 = (longlong)iVar1;
                *(short *)(iVar6 + iVar7) = (short)iVar1;
                uStack_64 = (int)*(short *)(iVar5 + iVar7) ^ 0x80000000;
                local_68 = 0x43300000;
                iVar1 = (int)(dVar15 * ((double)CONCAT44(0x43300000,uStack_64) - dVar10));
                local_80 = (double)(longlong)iVar1;
                *(short *)(iVar5 + iVar7) = (short)iVar1;
                iVar7 = iVar7 + 2;
                iVar9 = iVar9 + -1;
              } while (iVar9 != 0);
            }
            unaff_r20 = _AIFFSndAddStereo((int)_DAT_1000d678,iVar5,iVar6,iVar2,uVar4,auStack_88);
            if ((short)unaff_r20 != 0) {
              _DAT_1000d6c8 = 0x1e;
            }
            if (_DAT_1000d6c8 == 0) {
              uStack_74 = uVar3 ^ 0x80000000;
              _DAT_1000d6c8 = _DAT_1000d658;
              local_80 = (double)CONCAT44(0x43300000,uVar4 ^ 0x80000000);
              local_78 = 0x43300000;
              _UpdateConsole((double)((float)(local_80 - *(double *)(local_bc + 0x548)) /
                                     _DAT_1000d660),
                             (double)((float)((double)CONCAT44(0x43300000,uStack_74) -
                                             *(double *)(local_bc + 0x548)) / _DAT_1000d660),dVar14)
              ;
            }
            else {
              _DAT_1000d6c8 = _DAT_1000d6c8 + -1;
            }
            .glue::DisposePtr(iVar5);
            .glue::DisposePtr(iVar6);
          }
        }
      }
      .glue::DisposePtr(iVar5);
    }
  }
  return unaff_r20;
}




/* Função: .GetGrain | Endereço: 1000445c */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4
.debug::_GetGrain(double param_1,uint param_2,undefined4 param_3,undefined4 param_4,
                 undefined4 param_5,undefined4 param_6,int param_7)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  undefined4 uVar4;
  uint uVar5;
  int iVar6;
  double dVar7;
  double dVar8;
  double dVar9;
  int local_9c;
  uint local_78 [2];
  undefined8 local_70;
  undefined4 local_68;
  uint uStack_64;
  undefined4 local_60;
  uint uStack_5c;
  longlong local_58;
  undefined4 local_50;
  uint uStack_4c;
  
  dVar9 = _DAT_1000c438;
  uStack_4c = param_2 ^ 0x80000000;
  local_50 = 0x43300000;
  iVar6 = 0;
  dVar8 = param_1 * ((double)CONCAT44(0x43300000,uStack_4c) - _DAT_1000c408);
  iVar3 = (int)(_DAT_1000c418 * dVar8);
  local_58 = (longlong)iVar3;
  if (_DAT_1000c430 == param_1) {
    uVar4 = _AIFFSndRead(param_5,param_7,param_2,1,param_6,local_78);
  }
  else {
    iVar3 = .glue::NewPtr(iVar3);
    uVar4 = .glue::MemError();
    if ((short)uVar4 == 0) {
      iVar1 = (int)dVar8;
      local_58 = (longlong)iVar1;
      uVar4 = _AIFFSndRead(param_5,iVar3,iVar1,1,param_6,local_78);
      dVar8 = *(double *)(local_9c + 0x548);
      while( true ) {
        local_70 = (double)CONCAT44(0x43300000,local_78[0] ^ 0x80000000);
        if (local_70 - dVar8 < dVar9) break;
        uVar2 = (uint)dVar9;
        local_58 = (longlong)(int)uVar2;
        uVar5 = (uint)*(short *)(iVar3 + uVar2 * 2);
        uStack_4c = uVar2 ^ 0x80000000;
        uStack_64 = uVar5 ^ 0x80000000;
        local_50 = 0x43300000;
        uStack_5c = (int)*(short *)(iVar3 + (uVar2 + 1) * 2) - uVar5 ^ 0x80000000;
        local_60 = 0x43300000;
        local_68 = 0x43300000;
        dVar7 = dVar9 - ((double)CONCAT44(0x43300000,uStack_4c) - dVar8);
        dVar9 = dVar9 + param_1;
        *(short *)(param_7 + iVar6) =
             (short)(int)(dVar7 * ((double)CONCAT44(0x43300000,uStack_5c) - dVar8) +
                         ((double)CONCAT44(0x43300000,uStack_64) - dVar8));
        iVar6 = iVar6 + 2;
      }
      .glue::DisposePtr(iVar3);
    }
  }
  return uVar4;
}




/* Função: .ShapeGrain2 | Endereço: 1000464c */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_ShapeGrain2(double param_1,double param_2,double param_3,uint param_4)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  int in_r10;
  int iVar8;
  int iVar9;
  uint uVar10;
  double dVar11;
  double dVar12;
  double dVar13;
  double dVar14;
  double dVar15;
  double dVar16;
  undefined4 local_cc;
  undefined8 local_98;
  undefined8 local_90;
  undefined8 local_80;
  
  dVar16 = _DAT_1000c438;
  iVar8 = 0;
  if (_DAT_1000c430 < param_1 + param_2) {
    param_2 = _DAT_1000c430 - param_1;
  }
  uVar10 = (uint)(param_1 * ((double)CONCAT44(0x43300000,param_4 ^ 0x80000000) - _DAT_1000c408));
  uVar1 = (uint)(param_2 * ((double)CONCAT44(0x43300000,param_4 ^ 0x80000000) - _DAT_1000c408));
  iVar9 = (param_4 - uVar10) - uVar1;
  uVar2 = .glue::GetHandleSize(_DAT_1000d6c0);
  uVar3 = .glue::GetHandleSize(_DAT_1000d6bc);
  .glue::HLock(_DAT_1000d6c0);
  .glue::HLock(_DAT_1000d6bc);
  uVar5 = uVar10 ^ 0x80000000;
  dVar15 = *(double *)(local_cc + 0x548);
  iVar7 = *_DAT_1000d6c0;
  iVar4 = *_DAT_1000d6bc;
  local_80 = (double)CONCAT44(0x43300000,
                              ((int)uVar2 >> 1) + (uint)((int)uVar2 < 0 && (uVar2 & 1) != 0) ^
                              0x80000000);
  local_90 = (double)CONCAT44(0x43300000,
                              ((int)uVar3 >> 1) + (uint)((int)uVar3 < 0 && (uVar3 & 1) != 0) ^
                              0x80000000);
  local_98 = (double)CONCAT44(0x43300000,uVar1 ^ 0x80000000);
  dVar12 = local_90 - dVar15;
  dVar11 = local_98 - dVar15;
  if (uVar10 != 0) {
    iVar8 = 0;
    iVar6 = 0;
    dVar13 = *(double *)(local_cc + 0x540);
    dVar14 = dVar16;
    if (0 < (int)uVar10) {
      do {
        iVar8 = iVar8 + 1;
        local_90 = (double)CONCAT44(0x43300000,(int)*(short *)(iVar7 + (int)dVar14 * 2) ^ 0x80000000
                                   );
        *(short *)(in_r10 + iVar6) =
             (short)(int)(((double)CONCAT44(0x43300000,(int)*(short *)(in_r10 + iVar6) ^ 0x80000000)
                          - dVar15) * param_3 * ((local_90 - dVar15) / dVar13));
        iVar6 = iVar6 + 2;
        uVar10 = uVar10 - 1;
        dVar14 = dVar14 + (local_80 - dVar15) / ((double)CONCAT44(0x43300000,uVar5) - dVar15);
      } while (uVar10 != 0);
    }
  }
  if (iVar9 != 0) {
    if (*(double *)(local_cc + 0x570) == param_3) {
      iVar8 = iVar8 + iVar9;
    }
    else {
      dVar15 = *(double *)(local_cc + 0x548);
      iVar7 = iVar8 << 1;
      for (; iVar8 < (int)(param_4 - uVar1); iVar8 = iVar8 + 1) {
        local_98 = (double)CONCAT44(0x43300000,(int)*(short *)(in_r10 + iVar7) ^ 0x80000000);
        *(short *)(in_r10 + iVar7) = (short)(int)((local_98 - dVar15) * param_3);
        iVar7 = iVar7 + 2;
      }
    }
  }
  if (uVar1 != 0) {
    dVar14 = *(double *)(local_cc + 0x548);
    iVar7 = iVar8 << 1;
    dVar15 = *(double *)(local_cc + 0x540);
    for (; iVar8 < (int)param_4; iVar8 = iVar8 + 1) {
      iVar9 = (int)dVar16;
      dVar16 = dVar16 + dVar12 / dVar11;
      local_90 = (double)CONCAT44(0x43300000,(int)*(short *)(iVar4 + iVar9 * 2) ^ 0x80000000);
      *(short *)(in_r10 + iVar7) =
           (short)(int)(((double)CONCAT44(0x43300000,(int)*(short *)(in_r10 + iVar7) ^ 0x80000000) -
                        dVar14) * param_3 * ((local_90 - dVar14) / dVar15));
      iVar7 = iVar7 + 2;
    }
  }
  .glue::HUnlock(_DAT_1000d6c0);
  .glue::HUnlock(_DAT_1000d6bc);
  return;
}




/* Função: .InitEnvelops | Endereço: 10004990 */

void .debug::_InitEnvelops(int param_1,int param_2)

{
  undefined4 uVar1;
  undefined4 local_2c;
  
  uVar1 = .glue::NewHandle(param_1 << 1);
  *(undefined4 *)(local_2c + 0x1800) = uVar1;
  uVar1 = .glue::NewHandle(param_2 << 1);
  *(undefined4 *)(local_2c + 0x17fc) = uVar1;
  return;
}




/* Função: .IntToHandle | Endereço: 10004a00 */

void .debug::_IntToHandle(undefined4 param_1,int param_2,int *param_3)

{
  *(char *)(*param_3 + param_2) = (char)((uint)param_1 >> 8);
  *(char *)(*param_3 + param_2 + 1) = (char)param_1;
  return;
}




/* Função: .FRand | Endereço: 10004a44 */

undefined8 .debug::_FRand(void)

{
  short sVar1;
  short sVar2;
  double dVar3;
  undefined8 uVar4;
  double dVar5;
  undefined4 local_4c;
  
  sVar1 = .glue::Random();
  dVar5 = *(double *)(local_4c + 0x588);
  dVar3 = *(double *)(local_4c + 0x590);
  sVar2 = .glue::Random();
  uVar4 = _AbsDouble((((double)CONCAT44(0x43300000,(int)sVar1 ^ 0x80000000) - dVar5) / dVar3 +
                     ((double)CONCAT44(0x43300000,(int)sVar2 ^ 0x80000000) -
                     *(double *)(local_4c + 0x588))) / *(double *)(local_4c + 0x590));
  return uVar4;
}




/* Função: .AbsDouble | Endereço: 10004afc */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

double .debug::_AbsDouble(double param_1)

{
  if (param_1 < _DAT_1000c440) {
    param_1 = -param_1;
  }
  return param_1;
}




/* Função: .BorderLong | Endereço: 10004b2c */

int .debug::_BorderLong(int param_1,int param_2,int param_3)

{
  if (param_1 < param_2) {
    param_1 = param_2;
  }
  if (param_3 < param_1) {
    param_1 = param_3;
  }
  return param_1;
}




/* Função: .BorderDouble | Endereço: 10004b68 */

double .debug::_BorderDouble(double param_1,double param_2,double param_3)

{
  if (param_1 < param_2) {
    param_1 = param_2;
  }
  if (param_3 < param_1) {
    param_1 = param_3;
  }
  return param_1;
}




/* Função: .SetRandBP | Endereço: 10004ba4 */

void .debug::_SetRandBP(undefined8 param_1,undefined8 param_2)

{
  undefined8 *puVar1;
  undefined4 in_r7;
  
  puVar1 = (undefined8 *).glue::NewPtrClear(0x18);
  *puVar1 = param_2;
  puVar1[1] = param_1;
  _PutBPInList(puVar1,in_r7);
  return;
}




/* Função: .GLRnd | Endereço: 10004c28 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_GLRnd(void)

{
  _SetRandBP();
  return;
}




/* Função: .ARnd | Endereço: 10004c68 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_ARnd(void)

{
  _SetRandBP();
  return;
}




/* Função: .DRnd | Endereço: 10004ca8 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_DRnd(void)

{
  _SetRandBP();
  return;
}




/* Função: .FMRnd | Endereço: 10004ce8 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_FMRnd(void)

{
  _SetRandBP();
  return;
}




/* Função: .GRnd | Endereço: 10004d28 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_GRnd(void)

{
  _SetRandBP();
  return;
}




/* Função: .SORnd | Endereço: 10004d68 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_SORnd(void)

{
  _SetRandBP();
  return;
}




/* Função: .GFRnd | Endereço: 10004da8 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_GFRnd(void)

{
  _SetRandBP();
  return;
}




/* Função: .BRnd | Endereço: 10004de8 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_BRnd(void)

{
  _SetRandBP();
  return;
}




/* Função: .PutBPInList | Endereço: 10004e28 */

void .debug::_PutBPInList(undefined8 *param_1,undefined4 param_2)

{
  int iVar1;
  
  iVar1 = _GetInsertBP(*param_1,param_1,param_2,param_2);
  *(undefined4 *)(param_1 + 2) = *(undefined4 *)(iVar1 + 0x10);
  *(undefined8 **)(iVar1 + 0x10) = param_1;
  *(int *)((int)param_1 + 0x14) = iVar1;
  if (*(int *)(param_1 + 2) != 0) {
    *(undefined8 **)(*(int *)(param_1 + 2) + 0x14) = param_1;
  }
  return;
}




/* Função: .GetInsertBP | Endereço: 10004ea0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

double * .debug::_GetInsertBP(double param_1,undefined4 param_2,undefined4 param_3,double *param_4)

{
  double *pdVar1;
  
  pdVar1 = param_4;
  if (_DAT_1000c458 != param_1) {
    do {
      param_4 = pdVar1;
      if (param_1 <= *param_4) {
        if (*(double **)((int)param_4 + 0x14) != (double *)0x0) {
          return *(double **)((int)param_4 + 0x14);
        }
        return param_4;
      }
      pdVar1 = *(double **)(param_4 + 2);
    } while (*(double **)(param_4 + 2) != (double *)0x0);
  }
  return param_4;
}




/* Função: .PutFMBPInList | Endereço: 10004f18 */

void .debug::_PutFMBPInList(undefined8 *param_1)

{
  int iVar1;
  
  iVar1 = _GetInsertFMBP(*param_1);
  *(undefined4 *)(param_1 + 9) = *(undefined4 *)(iVar1 + 0x48);
  *(undefined8 **)(iVar1 + 0x48) = param_1;
  *(int *)((int)param_1 + 0x4c) = iVar1;
  if (*(int *)(param_1 + 9) != 0) {
    *(undefined8 **)(*(int *)(param_1 + 9) + 0x4c) = param_1;
  }
  return;
}




/* Função: .GetInsertFMBP | Endereço: 10004f8c */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

double * .debug::_GetInsertFMBP(double param_1)

{
  double *pdVar1;
  double *pdVar2;
  double *pdVar3;
  
  pdVar3 = _DAT_1000d69c;
  pdVar2 = _DAT_1000d69c;
  if (_DAT_1000c458 != param_1) {
    do {
      if (param_1 <= *pdVar2) {
        if (*(double **)((int)pdVar2 + 0x4c) != (double *)0x0) {
          return *(double **)((int)pdVar2 + 0x4c);
        }
        return pdVar2;
      }
      pdVar1 = pdVar2 + 9;
      pdVar3 = pdVar2;
      pdVar2 = *(double **)pdVar1;
    } while (*(double **)pdVar1 != (double *)0x0);
  }
  return pdVar3;
}




/* Função: .Rand | Endereço: 10005004 */

int .debug::_Rand(short param_1)

{
  uint uVar1;
  
  uVar1 = .glue::Random();
  return (int)(uVar1 & 0xffff) / (0x10000 / (int)param_1) + 1;
}




/* Função: .SetGL | Endereço: 10005064 */

void .debug::_SetGL(undefined8 param_1,uint param_2)

{
  undefined8 *puVar1;
  int local_4c;
  
  puVar1 = (undefined8 *).glue::NewPtrClear(0x18);
  *puVar1 = param_1;
  puVar1[1] = (double)CONCAT44(0x43300000,param_2 ^ 0x80000000) - *(double *)(local_4c + 0x5a8);
  _PutBPInList(puVar1,*(undefined4 *)(local_4c + 0x17f8));
  return;
}




/* Função: .SetA | Endereço: 100050f8 */

void .debug::_SetA(undefined8 param_1,undefined8 param_2)

{
  undefined8 *puVar1;
  int local_3c;
  
  puVar1 = (undefined8 *).glue::NewPtrClear(0x18);
  *puVar1 = param_2;
  puVar1[1] = param_1;
  _PutBPInList(puVar1,*(undefined4 *)(local_3c + 0x17f4));
  return;
}




/* Função: .SetD | Endereço: 10005170 */

void .debug::_SetD(undefined8 param_1,undefined8 param_2)

{
  undefined8 *puVar1;
  int local_3c;
  
  puVar1 = (undefined8 *).glue::NewPtrClear(0x18);
  *puVar1 = param_2;
  puVar1[1] = param_1;
  _PutBPInList(puVar1,*(undefined4 *)(local_3c + 0x17f0));
  return;
}




/* Função: .SetG | Endereço: 100051e8 */

void .debug::_SetG(undefined8 param_1,undefined8 param_2)

{
  undefined8 *puVar1;
  int local_3c;
  
  puVar1 = (undefined8 *).glue::NewPtrClear(0x18);
  *puVar1 = param_2;
  puVar1[1] = param_1;
  _PutBPInList(puVar1,*(undefined4 *)(local_3c + 0x17ec));
  return;
}




/* Função: .SetSO | Endereço: 10005260 */

void .debug::_SetSO(undefined8 param_1,undefined8 param_2)

{
  undefined8 *puVar1;
  int local_3c;
  
  puVar1 = (undefined8 *).glue::NewPtrClear(0x18);
  *puVar1 = param_2;
  puVar1[1] = param_1;
  _PutBPInList(puVar1,*(undefined4 *)(local_3c + 0x17e8));
  return;
}




/* Função: .SetGF | Endereço: 100052d8 */

void .debug::_SetGF(undefined8 param_1,undefined8 param_2)

{
  undefined8 *puVar1;
  int local_3c;
  
  puVar1 = (undefined8 *).glue::NewPtrClear(0x18);
  *puVar1 = param_2;
  puVar1[1] = param_1;
  _PutBPInList(puVar1,*(undefined4 *)(local_3c + 0x17e4));
  return;
}




/* Função: .SetB | Endereço: 10005350 */

void .debug::_SetB(double param_1,undefined8 param_2)

{
  undefined8 *puVar1;
  int local_3c;
  
  puVar1 = (undefined8 *).glue::NewPtrClear(0x18);
  *puVar1 = param_2;
  puVar1[1] = *(double *)(local_3c + 0x5a0) + param_1;
  _PutBPInList(puVar1,*(undefined4 *)(local_3c + 0x17e0));
  return;
}




/* Função: .SetFM | Endereço: 100053d0 */

void .debug::_SetFM(undefined8 param_1,undefined8 param_2,undefined8 param_3,undefined8 param_4,
                   undefined8 param_5,undefined4 param_6,undefined4 param_7,short param_8,
                   undefined4 param_9,undefined4 param_10,short param_11)

{
  undefined8 *puVar1;
  short in_stack_0000003a;
  short in_stack_00000046;
  int local_8c;
  
  puVar1 = (undefined8 *).glue::NewPtrClear(0x50);
  *puVar1 = param_5;
  puVar1[1] = param_1;
  puVar1[2] = param_2;
  puVar1[3] = param_3;
  puVar1[4] = param_4;
  puVar1[5] = (double)CONCAT44(0x43300000,(int)param_8 ^ 0x80000000) - *(double *)(local_8c + 0x5a8)
  ;
  puVar1[6] = (double)CONCAT44(0x43300000,(int)param_11 ^ 0x80000000) -
              *(double *)(local_8c + 0x5a8);
  puVar1[7] = (double)CONCAT44(0x43300000,(int)in_stack_0000003a ^ 0x80000000) -
              *(double *)(local_8c + 0x5a8);
  puVar1[8] = (double)CONCAT44(0x43300000,(int)in_stack_00000046 ^ 0x80000000) -
              *(double *)(local_8c + 0x5a8);
  _PutFMBPInList(puVar1);
  return;
}




/* Função: .SetDefaultValues | Endereço: 10005524 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_SetDefaultValues(void)

{
  undefined8 uVar1;
  undefined4 in_r6;
  undefined4 in_r7;
  undefined8 uVar2;
  
  uVar1 = _DAT_1000c490;
  _SetGF(_DAT_1000c488,_DAT_1000c490);
  _GFRnd(_DAT_1000c480,uVar1);
  _SetGL(uVar1,500);
  _GLRnd(_DAT_1000c480,uVar1);
  _SetA(_DAT_1000c478,uVar1);
  _ARnd(_DAT_1000c480,uVar1);
  _SetD(_DAT_1000c478,uVar1);
  _DRnd(_DAT_1000c480,uVar1);
  _SetG(_DAT_1000c470,uVar1);
  uVar2 = _GRnd(_DAT_1000c480,uVar1);
  _SetFM(_DAT_1000c480,_DAT_1000c480,_DAT_1000c480,_DAT_1000c480,uVar1,
         (int)((ulonglong)uVar2 >> 0x20),(int)uVar2,100,in_r6,in_r7,0);
  _FMRnd(_DAT_1000c480,uVar1);
  _SetSO(_DAT_1000c480,uVar1);
  _SORnd(_DAT_1000c480,uVar1);
  _SetB(_DAT_1000c480,uVar1);
  _BRnd(_DAT_1000c480,uVar1);
  return;
}




/* Função: .InitBP | Endereço: 10005690 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_InitBP(undefined8 *param_1)

{
  *param_1 = _DAT_1000c4a0;
  param_1[1] = _DAT_1000c498;
  *(undefined4 *)((int)param_1 + 0x14) = 0;
  *(undefined4 *)(param_1 + 2) = 0;
  return;
}




/* Função: .SetFirstBreakPoints | Endereço: 100056cc */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_SetFirstBreakPoints(void)

{
  _DAT_1000d6a4 = .glue::NewPtrClear(0x18);
  _InitBP(_DAT_1000d6a4);
  _DAT_1000d6b8 = .glue::NewPtrClear(0x18);
  _InitBP(_DAT_1000d6b8);
  _DAT_1000d6b4 = .glue::NewPtrClear(0x18);
  _InitBP(_DAT_1000d6b4);
  _DAT_1000d6b0 = .glue::NewPtrClear(0x18);
  _InitBP(_DAT_1000d6b0);
  _DAT_1000d6ac = .glue::NewPtrClear(0x18);
  _InitBP(_DAT_1000d6ac);
  _DAT_1000d6a8 = .glue::NewPtrClear(0x18);
  _InitBP(_DAT_1000d6a8);
  _DAT_1000d6a0 = .glue::NewPtrClear(0x18);
  _InitBP(_DAT_1000d6a0);
  _SetFirstFMBreakpoint();
  _DAT_1000d680 = .glue::NewPtrClear(0x18);
  _InitBP(_DAT_1000d680);
  _DAT_1000d698 = .glue::NewPtrClear(0x18);
  _InitBP(_DAT_1000d698);
  _DAT_1000d694 = .glue::NewPtrClear(0x18);
  _InitBP(_DAT_1000d694);
  _DAT_1000d690 = .glue::NewPtrClear(0x18);
  _InitBP(_DAT_1000d690);
  _DAT_1000d68c = .glue::NewPtrClear(0x18);
  _InitBP(_DAT_1000d68c);
  _DAT_1000d688 = .glue::NewPtrClear(0x18);
  _InitBP(_DAT_1000d688);
  _DAT_1000d684 = .glue::NewPtrClear(0x18);
  _InitBP(_DAT_1000d684);
  _DAT_1000d67c = .glue::NewPtrClear(0x18);
  _InitBP(_DAT_1000d67c);
  return;
}




/* Função: .SetFirstFMBreakpoint | Endereço: 100058c4 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_SetFirstFMBreakpoint(void)

{
  int local_2c;
  
  _DAT_1000d69c = (undefined8 *).glue::NewPtrClear(0x50);
  *_DAT_1000d69c = *(undefined8 *)(local_2c + 0x5e0);
  _DAT_1000d69c[1] = *(undefined8 *)(local_2c + 0x5e0);
  _DAT_1000d69c[2] = *(undefined8 *)(local_2c + 0x5e0);
  _DAT_1000d69c[3] = *(undefined8 *)(local_2c + 0x5e0);
  _DAT_1000d69c[4] = *(undefined8 *)(local_2c + 0x5e0);
  *(undefined4 *)((int)_DAT_1000d69c + 0x4c) = 0;
  *(undefined4 *)(_DAT_1000d69c + 9) = 0;
  return;
}




/* Função: .ShiftFirstBreakPoints | Endereço: 10005974 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_ShiftFirstBreakPoints(void)

{
  _DAT_1000d6b8 = *(undefined4 *)(_DAT_1000d6b8 + 0x10);
  _DAT_1000d6b4 = *(undefined4 *)(_DAT_1000d6b4 + 0x10);
  _DAT_1000d6b0 = *(undefined4 *)(_DAT_1000d6b0 + 0x10);
  _DAT_1000d69c = *(undefined4 *)(_DAT_1000d69c + 0x48);
  _DAT_1000d6ac = *(undefined4 *)(_DAT_1000d6ac + 0x10);
  _DAT_1000d6a8 = *(undefined4 *)(_DAT_1000d6a8 + 0x10);
  _DAT_1000d6a4 = *(undefined4 *)(_DAT_1000d6a4 + 0x10);
  _DAT_1000d6a0 = *(undefined4 *)(_DAT_1000d6a0 + 0x10);
  _DAT_1000d698 = *(undefined4 *)(_DAT_1000d698 + 0x10);
  _DAT_1000d694 = *(undefined4 *)(_DAT_1000d694 + 0x10);
  _DAT_1000d690 = *(undefined4 *)(_DAT_1000d690 + 0x10);
  _DAT_1000d68c = *(undefined4 *)(_DAT_1000d68c + 0x10);
  _DAT_1000d688 = *(undefined4 *)(_DAT_1000d688 + 0x10);
  _DAT_1000d684 = *(undefined4 *)(_DAT_1000d684 + 0x10);
  _DAT_1000d680 = *(undefined4 *)(_DAT_1000d680 + 0x10);
  _DAT_1000d67c = *(undefined4 *)(_DAT_1000d67c + 0x10);
  return;
}




/* Função: .DisposLinkedList | Endereço: 10005ab0 */

void .debug::_DisposLinkedList(int param_1)

{
  int iVar1;
  
  if (*(int *)(param_1 + 0x14) != 0) {
    param_1 = *(int *)(param_1 + 0x14);
  }
  while (*(int *)(param_1 + 0x10) != 0) {
    iVar1 = *(int *)(param_1 + 0x10);
    .glue::DisposePtr(param_1);
    param_1 = iVar1;
  }
  return;
}




/* Função: .DisposFMLinkedList | Endereço: 10005b3c */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_DisposFMLinkedList(void)

{
  int iVar1;
  int iVar2;
  
  iVar1 = _DAT_1000d69c;
  if (*(int *)(_DAT_1000d69c + 0x4c) != 0) {
    iVar1 = *(int *)(_DAT_1000d69c + 0x4c);
  }
  while (*(int *)(iVar1 + 0x48) != 0) {
    iVar2 = *(int *)(iVar1 + 0x48);
    .glue::DisposePtr(iVar1);
    iVar1 = iVar2;
  }
  return;
}




/* Função: .StopGrainDamage | Endereço: 10005bcc */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_StopGrainDamage(void)

{
  undefined4 local_2c;
  
  .glue::DisposeHandle(_DAT_1000d6c0);
  .glue::DisposeHandle(*(undefined4 *)(local_2c + 0x17fc));
  _DisposLinkedList(*(undefined4 *)(local_2c + 0x17f8));
  _DisposLinkedList(*(undefined4 *)(local_2c + 0x17f4));
  _DisposLinkedList(*(undefined4 *)(local_2c + 0x17f0));
  _DisposLinkedList(*(undefined4 *)(local_2c + 0x17ec));
  _DisposLinkedList(*(undefined4 *)(local_2c + 0x17e8));
  _DisposLinkedList(*(undefined4 *)(local_2c + 0x17e4));
  _DisposLinkedList(*(undefined4 *)(local_2c + 0x17e0));
  _DisposFMLinkedList();
  _DisposLinkedList(*(undefined4 *)(local_2c + 0x17d8));
  _DisposLinkedList(*(undefined4 *)(local_2c + 0x17d4));
  _DisposLinkedList(*(undefined4 *)(local_2c + 0x17d0));
  _DisposLinkedList(*(undefined4 *)(local_2c + 0x17cc));
  _DisposLinkedList(*(undefined4 *)(local_2c + 0x17c8));
  _DisposLinkedList(*(undefined4 *)(local_2c + 0x17c4));
  _DisposLinkedList(*(undefined4 *)(local_2c + 0x17c0));
  _DisposLinkedList(*(undefined4 *)(local_2c + 0x17bc));
  if (_DAT_1000d65c != 0) {
    .glue::DisposeDialog();
  }
  return;
}




/* Função: .ThonkDialog | Endereço: 10005d04 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint .debug::_ThonkDialog(void)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  uint uVar4;
  int iVar5;
  int local_ac;
  undefined4 local_88;
  char local_84 [2];
  undefined4 local_82;
  undefined4 local_7e;
  undefined4 local_7a;
  undefined4 local_76;
  undefined4 local_72;
  undefined4 local_6e;
  undefined1 auStack_6a [8];
  undefined1 auStack_62 [2];
  short local_60;
  undefined1 auStack_5e [2];
  undefined4 local_5c;
  ushort local_58;
  
  local_58 = 0;
  uVar1 = .glue::NewRoutineDescriptor(PTR_PTR_1000c144,0xfd0,1);
  .glue::GetPort(&local_88);
  uVar2 = .glue::GetNewDialog(0x81,local_58,0xffffffff);
  .glue::SetWTitle(uVar2,local_ac + 0x90a);
  .glue::SetPort(uVar2);
  .glue::TextFont(0x16);
  .glue::TextSize(0xc);
  .glue::GetDialogItem(uVar2,1,auStack_5e,&local_5c,&local_7a);
  .glue::GetDialogItem(uVar2,2,auStack_5e,&local_5c,&local_72);
  .glue::GetDialogItem(uVar2,3,auStack_5e,&local_5c,&local_82);
  .glue::ShowWindow(uVar2);
  .glue::DrawControls(uVar2);
  do {
    _NoModalDialog(uVar1,&local_60);
    iVar5 = (int)local_60;
    if (iVar5 == 3) {
      _DoCustomButton(*(undefined4 *)(local_ac + 0x176c),_DAT_1000d628,local_82,local_7e,local_84);
      if (local_84[0] == '\0') {
        local_60 = -1;
      }
    }
    else if (iVar5 < 3) {
      if (iVar5 == 1) {
        _DoCustomButton(*(undefined4 *)(local_ac + 0x177c),_DAT_1000d638,local_7a,local_76,local_84)
        ;
        if (local_84[0] == '\0') {
          local_60 = -1;
        }
      }
      else if ((0 < iVar5) &&
              (_DoCustomButton(*(undefined4 *)(local_ac + 0x1774),_DAT_1000d630,local_72,local_6e,
                               local_84), local_84[0] != '\0')) {
        uVar3 = .glue::GetNewDialog(500,0,0xffffffff);
        .glue::ModalDialog(0,auStack_62);
        .glue::DisposeDialog(uVar3);
      }
    }
    else if (iVar5 < 5) {
      .glue::GetDialogItem(uVar2,iVar5,auStack_5e,&local_5c,auStack_6a);
      _DAT_1000c6aa = .glue::GetControlValue(local_5c);
    }
    if (_DAT_1000c7c8 != _DAT_1000c6aa) {
      .glue::DrawPicture(_DAT_1000d638,&local_7a);
      .glue::DrawPicture(_DAT_1000d630,&local_72);
      .glue::DrawPicture(_DAT_1000d628,&local_82);
      .glue::DrawControls(uVar2);
      _DAT_1000c7c8 = _DAT_1000c6aa;
      .glue::GetDialogItem(uVar2,6,auStack_5e,&local_5c,auStack_6a);
      .glue::DrawPicture(*(undefined4 *)(local_ac + 0x1764),auStack_6a);
      .glue::MoveTo(0x19,0xc1);
      if ((uint)(int)_DAT_1000c6aa < 0x12) {
                    /* WARNING: Could not recover jumptable at 0x1000602c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
        uVar4 = (**(code **)(local_ac + 0xe64 + _DAT_1000c6aa * 4))();
        return uVar4;
      }
      .glue::DrawString(local_ac + 0xe55);
    }
  } while ((local_60 != 1) && (local_60 != 3));
  if (local_60 == 3) {
    local_58 = 0xb1e0;
  }
  .glue::DisposeDialog(uVar2);
  .glue::SetPort(local_88);
  return (uint)local_58;
}




/* Função: .DoCustomButton | Endereço: 100062a8 */

void .debug::_DoCustomButton
               (undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
               undefined1 *param_5)

{
  undefined4 uVar1;
  char cVar2;
  undefined4 uStack00000020;
  undefined4 uStack00000024;
  undefined4 local_18;
  
  uStack00000020 = param_3;
  uStack00000024 = param_4;
  .glue::DrawPicture(param_1,&stack0x00000020);
  *param_5 = 1;
  while (cVar2 = .glue::Button(), cVar2 != '\0') {
    .glue::GetMouse(&local_18);
    uVar1 = .glue::PinRect(&stack0x00000020,local_18);
    if ((local_18._0_2_ == (short)((uint)uVar1 >> 0x10)) && (local_18._2_2_ == (short)uVar1)) {
      .glue::DrawPicture(param_1,&stack0x00000020);
      *param_5 = 1;
      while (((local_18._0_2_ == (short)((uint)uVar1 >> 0x10) && (local_18._2_2_ == (short)uVar1))
             && (cVar2 = .glue::Button(), cVar2 != '\0'))) {
        .glue::GetMouse(&local_18);
        uVar1 = .glue::PinRect(&stack0x00000020,local_18);
      }
    }
    else {
      .glue::DrawPicture(param_2,&stack0x00000020);
      *param_5 = 0;
      while (((local_18._0_2_ != (short)((uint)uVar1 >> 0x10) && (local_18._2_2_ != (short)uVar1))
             && (cVar2 = .glue::Button(), cVar2 != '\0'))) {
        .glue::GetMouse(&local_18);
        uVar1 = .glue::PinRect(&stack0x00000020,local_18);
      }
    }
  }
  .glue::DrawPicture(param_2,&stack0x00000020);
  return;
}




/* Função: .NoModalDialog | Endereço: 100064bc */

void .debug::_NoModalDialog(undefined4 param_1,undefined2 *param_2)

{
  char cVar4;
  undefined4 uVar1;
  int iVar2;
  short sVar3;
  int local_6c;
  undefined4 local_48;
  undefined1 auStack_44 [8];
  undefined4 local_3c;
  undefined4 local_38;
  short local_24 [5];
  undefined4 local_1a;
  
  *param_2 = 0xffff;
  .glue::SystemTask();
  .glue::WaitNextEvent(0xffff,local_24,0,0);
  cVar4 = .glue::IsDialogEvent(local_24);
  if (cVar4 == '\0') {
    if (local_24[0] == 1) {
      sVar3 = .glue::FindWindow(local_1a,&local_48);
      if (sVar3 == 4) {
        local_3c = *(undefined4 *)(local_6c + 0x1be6);
        local_38 = *(undefined4 *)(local_6c + 0x1bea);
        .glue::DragWindow(local_48,local_1a,&local_3c);
      }
      else if ((sVar3 < 4) && (sVar3 == 1)) {
        .glue::MenuSelect(local_1a);
        cVar4 = _HandleThonkMenu();
        if (cVar4 != '\0') {
          *param_2 = 3;
        }
      }
    }
  }
  else {
    uVar1 = .glue::FrontWindow();
    iVar2 = .glue::CallUniversalProc(param_1,0xfd0,uVar1,local_24,param_2);
    if (iVar2 == 0) {
      .glue::DialogSelect(local_24,auStack_44,param_2);
    }
  }
  return;
}




/* Função: .HandleThonkMenu | Endereço: 10006634 */

undefined4 .debug::_HandleThonkMenu(int param_1)

{
  undefined4 uVar1;
  short sVar2;
  undefined4 uVar3;
  undefined1 auStack_118 [2];
  undefined1 auStack_116 [270];
  
  sVar2 = (short)((uint)param_1 >> 0x10);
  uVar3 = 0;
  if (sVar2 == 0x81) {
    if ((short)param_1 == 1) {
      uVar3 = 1;
    }
  }
  else if ((sVar2 < 0x81) && (0x7f < sVar2)) {
    if ((short)param_1 == 1) {
      uVar1 = .glue::GetNewDialog(500,0,0xffffffff);
      .glue::ModalDialog(0,auStack_118);
      .glue::DisposeDialog(uVar1);
    }
    else {
      uVar1 = .glue::GetMenuHandle(param_1 >> 0x10);
      .glue::GetMenuItemText(uVar1,param_1,auStack_116);
      .glue::OpenDeskAcc(auStack_116);
    }
  }
  .glue::HiliteMenu(0);
  return uVar3;
}




/* Função: .StartGrainDamage | Endereço: 10006a24 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_StartGrainDamage(void)

{
  _InitEnvelops(1000,1000);
  _MakeCosineAttack();
  _MakeCosineDecay();
  DAT_1000c6a8 = 1;
  _DAT_1000d658 = 10;
  return;
}




/* Função: .Dis | Endereço: 10006a9c */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_Dis(void)

{
  uint uVar1;
  float fVar2;
  short sVar4;
  int iVar3;
  undefined4 uVar5;
  undefined4 extraout_r4;
  undefined4 in_r6;
  undefined4 in_r7;
  short sVar6;
  double dVar7;
  double dVar8;
  double dVar9;
  double extraout_f1;
  double dVar10;
  double dVar11;
  double dVar12;
  double dVar13;
  double dVar14;
  double dVar15;
  double dVar16;
  double dVar17;
  double dVar18;
  double dVar19;
  double dVar20;
  undefined4 local_fc;
  undefined8 local_a8;
  
  dVar20 = (double)_DAT_1000c63c;
  .glue::GetDateTime(0x1000da9c);
  uVar1 = (uint)(dVar20 / (double)*(float *)(local_fc + 0x778));
  dVar15 = *(double *)(local_fc + 0x770);
  sVar4 = (short)uVar1;
  dVar14 = *(double *)(local_fc + 0x6d8);
  dVar13 = *(double *)(local_fc + 0x768);
  dVar12 = *(double *)(local_fc + 0x760);
  dVar16 = *(double *)(local_fc + 0x758);
  dVar11 = *(double *)(local_fc + 0x750);
  dVar10 = *(double *)(local_fc + 0x748);
  for (sVar6 = 0; sVar6 <= sVar4; sVar6 = sVar6 + 1) {
    dVar17 = (double)_FRand();
    dVar18 = (double)_FRand();
    iVar3 = (int)(short)(int)(dVar15 * dVar17);
    local_a8 = (double)CONCAT44(0x43300000,iVar3 * iVar3 ^ 0x80000000);
    _SetGF(local_a8 - dVar14,dVar20 * dVar18);
    dVar17 = (double)_FRand();
    dVar18 = (double)_FRand();
    iVar3 = (int)(short)(int)(dVar15 * dVar17);
    _SetGF((double)CONCAT44(0x43300000,iVar3 * iVar3 ^ 0x80000000) - dVar14,dVar20 * dVar18);
    dVar17 = (double)_FRand();
    dVar18 = (double)_FRand();
    _GFRnd(dVar13 * dVar18,dVar20 * dVar17);
    dVar17 = (double)_FRand();
    dVar18 = (double)_FRand();
    _SetGL(dVar20 * dVar17,(int)(dVar12 * dVar18));
    dVar17 = (double)_FRand();
    dVar18 = (double)_FRand();
    _GLRnd(dVar13 * dVar18,dVar20 * dVar17);
    dVar17 = (double)_FRand();
    dVar18 = (double)_FRand();
    _SetB(dVar16 * dVar18 - dVar11,dVar20 * dVar17);
    dVar17 = (double)_FRand();
    dVar18 = (double)_FRand();
    _BRnd(dVar10 * dVar18,dVar20 * dVar17);
  }
  dVar11 = *(double *)(local_fc + 0x738);
  dVar12 = *(double *)(local_fc + 0x740);
  dVar13 = *(double *)(local_fc + 0x730);
  dVar14 = *(double *)(local_fc + 0x6d8);
  dVar15 = *(double *)(local_fc + 0x728);
  dVar16 = *(double *)(local_fc + 0x720);
  dVar17 = *(double *)(local_fc + 0x708);
  dVar18 = *(double *)(local_fc + 0x710);
  dVar19 = *(double *)(local_fc + 0x718);
  fVar2 = *(float *)(local_fc + 0x700);
  dVar10 = *(double *)(local_fc + 0x758);
  for (sVar6 = 0; sVar6 <= (short)((sVar4 >> 1) + (ushort)(sVar4 < 0 && (uVar1 & 1) != 0));
      sVar6 = sVar6 + 1) {
    dVar7 = (double)_FRand();
    dVar8 = (double)_FRand();
    dVar9 = (double)_FRand();
    _SetSO((_DAT_1000d650 - dVar13) * dVar9,dVar20 * dVar8);
    dVar8 = (double)_FRand();
    dVar9 = (double)_FRand();
    iVar3 = (int)(short)(int)(dVar11 * dVar7 + dVar12);
    _SORnd((((double)CONCAT44(0x43300000,iVar3 * iVar3 ^ 0x80000000) - dVar14) * dVar9) / dVar15,
           dVar20 * dVar8);
    dVar7 = (double)_FRand();
    dVar7 = dVar16 * dVar7;
    dVar8 = (double)_FRand();
    dVar9 = (double)_FRand();
    uVar5 = _FRand();
    _SetFM(dVar18 * extraout_f1 + dVar19,dVar11 * dVar9 + dVar17,*(undefined8 *)(local_fc + 0x6f8),
           *(undefined8 *)(local_fc + 0x6f8),dVar20 * dVar8,uVar5,extraout_r4,(int)dVar7,in_r6,in_r7
           ,(int)(fVar2 - (float)dVar7));
    dVar7 = (double)_FRand();
    dVar8 = (double)_FRand();
    _FMRnd(dVar10 * dVar8,dVar20 * dVar7);
  }
  _SetGF(*(undefined8 *)(local_fc + 0x728),*(undefined8 *)(local_fc + 0x6f8));
  _SetSO(*(undefined8 *)(local_fc + 0x6f0),*(undefined8 *)(local_fc + 0x6f8));
  _SetGL(*(undefined8 *)(local_fc + 0x6f8),2000);
  _SetA(*(undefined8 *)(local_fc + 0x6f0),*(undefined8 *)(local_fc + 0x6f8));
  _ARnd(*(undefined8 *)(local_fc + 0x6f8),*(undefined8 *)(local_fc + 0x6f8));
  _SetD(*(undefined8 *)(local_fc + 0x6f0),*(undefined8 *)(local_fc + 0x6f8));
  _DRnd(*(undefined8 *)(local_fc + 0x6f8),*(undefined8 *)(local_fc + 0x6f8));
  _SetG(*(undefined8 *)(local_fc + 0x6e8),*(undefined8 *)(local_fc + 0x6e0));
  _GRnd(*(undefined8 *)(local_fc + 0x6f8),*(undefined8 *)(local_fc + 0x6f8));
  _BreakPointsToSoundFile(*(undefined8 *)(local_fc + 0x6f8),dVar20);
  return;
}




/* Função: .ClassicScoreHectic | Endereço: 10006f30 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_ClassicScoreHectic(void)

{
  uint uVar1;
  int iVar2;
  float fVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  undefined4 extraout_r4;
  undefined4 in_r6;
  undefined4 in_r7;
  short sVar7;
  short sVar8;
  double dVar9;
  double dVar10;
  double dVar11;
  double extraout_f1;
  double dVar12;
  double dVar13;
  double dVar14;
  double dVar15;
  double dVar16;
  double dVar17;
  double dVar18;
  double dVar19;
  double dVar20;
  double dVar21;
  double dVar22;
  int local_10c;
  undefined4 local_d0 [2];
  longlong local_c8;
  undefined4 local_c0;
  uint uStack_bc;
  longlong local_b8;
  undefined8 local_b0;
  longlong local_a8;
  
  dVar22 = (double)_DAT_1000c63c;
  .glue::GetDateTime(0x1000da9c);
  DAT_1000d64c = '\0';
  uVar1 = (uint)(dVar22 * (double)*(float *)(local_10c + 0x6d0));
  local_a8 = (longlong)(int)uVar1;
  sVar7 = (short)uVar1;
  .glue::GetPort(local_d0);
  iVar5 = .glue::GetNewDialog(200,0,0xffffffff);
  .glue::SetPort(iVar5);
  .glue::DrawDialog(iVar5);
  dVar17 = *(double *)(local_10c + 0x770);
  dVar16 = *(double *)(local_10c + 0x6d8);
  dVar15 = *(double *)(local_10c + 0x768);
  dVar14 = *(double *)(local_10c + 0x760);
  dVar18 = *(double *)(local_10c + 0x758);
  dVar13 = *(double *)(local_10c + 0x750);
  dVar12 = *(double *)(local_10c + 0x748);
  for (sVar8 = 0; sVar8 <= sVar7; sVar8 = sVar8 + 1) {
    dVar19 = (double)_FRand();
    iVar4 = (int)(dVar17 * dVar19);
    local_a8 = (longlong)iVar4;
    dVar19 = (double)_FRand();
    iVar4 = (int)(short)iVar4;
    local_b0 = (double)CONCAT44(0x43300000,iVar4 * iVar4 ^ 0x80000000);
    _SetGF(local_b0 - dVar16,dVar22 * dVar19);
    dVar19 = (double)_FRand();
    iVar4 = (int)(dVar17 * dVar19);
    local_b8 = (longlong)iVar4;
    dVar19 = (double)_FRand();
    iVar4 = (int)(short)iVar4;
    uStack_bc = iVar4 * iVar4 ^ 0x80000000;
    local_c0 = 0x43300000;
    _SetGF((double)CONCAT44(0x43300000,uStack_bc) - dVar16,dVar22 * dVar19);
    dVar19 = (double)_FRand();
    dVar20 = (double)_FRand();
    _GFRnd(dVar15 * dVar20,dVar22 * dVar19);
    dVar19 = (double)_FRand();
    dVar20 = (double)_FRand();
    iVar4 = (int)(dVar14 * dVar20);
    local_c8 = (longlong)iVar4;
    _SetGL(dVar22 * dVar19,iVar4);
    dVar19 = (double)_FRand();
    dVar20 = (double)_FRand();
    _GLRnd(dVar15 * dVar20,dVar22 * dVar19);
    dVar19 = (double)_FRand();
    dVar20 = (double)_FRand();
    _SetB(dVar18 * dVar20 - dVar13,dVar22 * dVar19);
    dVar19 = (double)_FRand();
    dVar20 = (double)_FRand();
    _BRnd(dVar12 * dVar20,dVar22 * dVar19);
  }
  if (DAT_1000d64c != '\0') {
    sVar7 = (sVar7 >> 1) + (ushort)(sVar7 < 0 && (uVar1 & 1) != 0);
  }
  dVar13 = *(double *)(local_10c + 0x738);
  dVar14 = *(double *)(local_10c + 0x740);
  dVar15 = *(double *)(local_10c + 0x730);
  dVar16 = *(double *)(local_10c + 0x6d8);
  dVar17 = *(double *)(local_10c + 0x728);
  dVar18 = *(double *)(local_10c + 0x720);
  dVar19 = *(double *)(local_10c + 0x6c0);
  dVar20 = *(double *)(local_10c + 0x710);
  dVar21 = *(double *)(local_10c + 0x6c8);
  fVar3 = *(float *)(local_10c + 0x700);
  dVar12 = *(double *)(local_10c + 0x758);
  for (sVar8 = 0; sVar8 <= sVar7; sVar8 = sVar8 + 1) {
    dVar9 = (double)_FRand();
    iVar4 = (int)(dVar13 * dVar9 + dVar14);
    local_c8 = (longlong)iVar4;
    dVar9 = (double)_FRand();
    dVar10 = (double)_FRand();
    _SetSO((_DAT_1000d650 - dVar15) * dVar10,dVar22 * dVar9);
    dVar9 = (double)_FRand();
    dVar10 = (double)_FRand();
    iVar4 = (int)(short)iVar4;
    uStack_bc = iVar4 * iVar4 ^ 0x80000000;
    local_c0 = 0x43300000;
    _SORnd((((double)CONCAT44(0x43300000,uStack_bc) - dVar16) * dVar10) / dVar17,dVar22 * dVar9);
    dVar9 = (double)_FRand();
    dVar9 = dVar18 * dVar9;
    dVar10 = (double)_FRand();
    dVar11 = (double)_FRand();
    uVar6 = _FRand();
    iVar4 = (int)dVar9;
    local_b8 = (longlong)iVar4;
    iVar2 = (int)(fVar3 - (float)dVar9);
    local_b0 = (double)(longlong)iVar2;
    _SetFM(dVar20 * extraout_f1 + dVar21,dVar19 * dVar11 + dVar19,*(undefined8 *)(local_10c + 0x6f8)
           ,*(undefined8 *)(local_10c + 0x6f8),dVar22 * dVar10,uVar6,extraout_r4,iVar4,in_r6,in_r7,
           iVar2);
    dVar9 = (double)_FRand();
    dVar10 = (double)_FRand();
    _FMRnd(dVar12 * dVar10,dVar22 * dVar9);
  }
  _SetGF(*(undefined8 *)(local_10c + 0x768),*(undefined8 *)(local_10c + 0x6f8));
  _SetSO(*(undefined8 *)(local_10c + 0x6f0),*(undefined8 *)(local_10c + 0x6f8));
  _SetGL(*(undefined8 *)(local_10c + 0x6f8),200);
  _SetA(*(undefined8 *)(local_10c + 0x6f0),*(undefined8 *)(local_10c + 0x6f8));
  _ARnd(*(undefined8 *)(local_10c + 0x6f8),*(undefined8 *)(local_10c + 0x6f8));
  _SetD(*(undefined8 *)(local_10c + 0x6f0),*(undefined8 *)(local_10c + 0x6f8));
  _DRnd(*(undefined8 *)(local_10c + 0x6f8),*(undefined8 *)(local_10c + 0x6f8));
  _SetG(*(undefined8 *)(local_10c + 0x6b8),*(undefined8 *)(local_10c + 0x6e0));
  _GRnd(*(undefined8 *)(local_10c + 0x6f8),*(undefined8 *)(local_10c + 0x6f8));
  if (iVar5 != 0) {
    .glue::DisposeDialog(iVar5);
  }
  .glue::SetPort(local_d0[0]);
  _BreakPointsToSoundFile(*(undefined8 *)(local_10c + 0x6f8),dVar22);
  return;
}




/* Função: .Flash | Endereço: 10007448 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_Flash(void)

{
  uint uVar1;
  float fVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 extraout_r4;
  undefined4 in_r6;
  undefined4 in_r7;
  short sVar6;
  short sVar7;
  double dVar8;
  double dVar9;
  double dVar10;
  double extraout_f1;
  double dVar11;
  double dVar12;
  double dVar13;
  double dVar14;
  double dVar15;
  double dVar16;
  double dVar17;
  double dVar18;
  double dVar19;
  double dVar20;
  double dVar21;
  int local_10c;
  undefined4 local_d0 [2];
  longlong local_c8;
  undefined8 local_c0;
  longlong local_b8;
  undefined4 local_b0;
  uint uStack_ac;
  longlong local_a8;
  
  dVar21 = (double)_DAT_1000c63c;
  .glue::GetDateTime(0x1000da9c);
  DAT_1000d64c = '\0';
  uVar1 = (uint)((double)*(float *)(local_10c + 0x6b4) * dVar21);
  local_a8 = (longlong)(int)uVar1;
  sVar6 = (short)uVar1;
  .glue::GetPort(local_d0);
  iVar4 = .glue::GetNewDialog(200,0,0xffffffff);
  .glue::SetPort(iVar4);
  .glue::DrawDialog(iVar4);
  dVar16 = *(double *)(local_10c + 0x770);
  dVar15 = *(double *)(local_10c + 0x6d8);
  dVar14 = *(double *)(local_10c + 0x768);
  dVar13 = *(double *)(local_10c + 0x760);
  dVar17 = *(double *)(local_10c + 0x758);
  dVar12 = *(double *)(local_10c + 0x750);
  dVar11 = *(double *)(local_10c + 0x748);
  for (sVar7 = 0; sVar7 <= sVar6; sVar7 = sVar7 + 1) {
    dVar18 = (double)_FRand();
    iVar3 = (int)(dVar16 * dVar18);
    local_a8 = (longlong)iVar3;
    dVar18 = (double)_FRand();
    iVar3 = (int)(short)iVar3;
    uStack_ac = iVar3 * iVar3 ^ 0x80000000;
    local_b0 = 0x43300000;
    _SetGF((double)CONCAT44(0x43300000,uStack_ac) - dVar15,dVar21 * dVar18);
    dVar18 = (double)_FRand();
    iVar3 = (int)(dVar16 * dVar18);
    local_b8 = (longlong)iVar3;
    dVar18 = (double)_FRand();
    iVar3 = (int)(short)iVar3;
    local_c0 = (double)CONCAT44(0x43300000,iVar3 * iVar3 ^ 0x80000000);
    _SetGF(local_c0 - dVar15,dVar21 * dVar18);
    dVar18 = (double)_FRand();
    dVar19 = (double)_FRand();
    _GFRnd(dVar14 * dVar19,dVar21 * dVar18);
    dVar18 = (double)_FRand();
    dVar19 = (double)_FRand();
    iVar3 = (int)(dVar13 * dVar19);
    local_c8 = (longlong)iVar3;
    _SetGL(dVar21 * dVar18,iVar3);
    dVar18 = (double)_FRand();
    dVar19 = (double)_FRand();
    _GLRnd(dVar14 * dVar19,dVar21 * dVar18);
    dVar18 = (double)_FRand();
    dVar19 = (double)_FRand();
    _SetB(dVar17 * dVar19 - dVar12,dVar21 * dVar18);
    dVar18 = (double)_FRand();
    dVar19 = (double)_FRand();
    _BRnd(dVar11 * dVar19,dVar21 * dVar18);
  }
  if (DAT_1000d64c != '\0') {
    sVar6 = (sVar6 >> 1) + (ushort)(sVar6 < 0 && (uVar1 & 1) != 0);
  }
  dVar12 = *(double *)(local_10c + 0x738);
  dVar13 = *(double *)(local_10c + 0x740);
  dVar14 = *(double *)(local_10c + 0x730);
  dVar15 = *(double *)(local_10c + 0x6d8);
  dVar16 = *(double *)(local_10c + 0x728);
  dVar17 = *(double *)(local_10c + 0x720);
  dVar18 = *(double *)(local_10c + 0x6c0);
  dVar19 = *(double *)(local_10c + 0x710);
  dVar20 = *(double *)(local_10c + 0x6c8);
  fVar2 = *(float *)(local_10c + 0x700);
  dVar11 = *(double *)(local_10c + 0x758);
  for (sVar7 = 0; sVar7 <= sVar6; sVar7 = sVar7 + 1) {
    dVar8 = (double)_FRand();
    iVar3 = (int)(dVar12 * dVar8 + dVar13);
    local_c8 = (longlong)iVar3;
    dVar8 = (double)_FRand();
    dVar9 = (double)_FRand();
    _SetSO((_DAT_1000d650 - dVar14) * dVar9,dVar21 * dVar8);
    dVar8 = (double)_FRand();
    dVar9 = (double)_FRand();
    iVar3 = (int)(short)iVar3;
    local_c0 = (double)CONCAT44(0x43300000,iVar3 * iVar3 ^ 0x80000000);
    _SORnd(((local_c0 - dVar15) * dVar9) / dVar16,dVar21 * dVar8);
    dVar8 = (double)_FRand();
    dVar9 = (double)_FRand();
    dVar9 = (double)(float)(dVar17 * dVar8) * dVar9;
    dVar8 = (double)(float)dVar9;
    uVar1 = (uint)dVar9;
    local_b8 = (longlong)(int)uVar1;
    if (uVar1 != (((int)uVar1 >> 1) + (uint)((int)uVar1 < 0 && (uVar1 & 1) != 0)) * 2) {
      dVar8 = (double)(float)(dVar17 - dVar8);
    }
    dVar9 = (double)_FRand();
    dVar10 = (double)_FRand();
    uVar5 = _FRand();
    local_c8 = (longlong)(int)dVar8;
    iVar3 = (int)((double)fVar2 - dVar8);
    local_c0 = (double)(longlong)iVar3;
    _SetFM(dVar19 * extraout_f1 + dVar20,dVar18 * dVar10,*(undefined8 *)(local_10c + 0x6f8),
           *(undefined8 *)(local_10c + 0x6f8),dVar21 * dVar9,uVar5,extraout_r4,(int)dVar8,in_r6,
           in_r7,iVar3);
    dVar8 = (double)_FRand();
    dVar9 = (double)_FRand();
    _FMRnd(dVar11 * dVar9,dVar21 * dVar8);
  }
  _SetGF(*(undefined8 *)(local_10c + 0x768),*(undefined8 *)(local_10c + 0x6f8));
  _SetSO(*(undefined8 *)(local_10c + 0x6f0),*(undefined8 *)(local_10c + 0x6f8));
  _SetGL(*(undefined8 *)(local_10c + 0x6f8),200);
  _SetA(*(undefined8 *)(local_10c + 0x6f0),*(undefined8 *)(local_10c + 0x6f8));
  _ARnd(*(undefined8 *)(local_10c + 0x6f8),*(undefined8 *)(local_10c + 0x6f8));
  _SetD(*(undefined8 *)(local_10c + 0x6f0),*(undefined8 *)(local_10c + 0x6f8));
  _DRnd(*(undefined8 *)(local_10c + 0x6f8),*(undefined8 *)(local_10c + 0x6f8));
  _SetG(*(undefined8 *)(local_10c + 0x6b8),*(undefined8 *)(local_10c + 0x6e0));
  _GRnd(*(undefined8 *)(local_10c + 0x6f8),*(undefined8 *)(local_10c + 0x6f8));
  if (iVar4 != 0) {
    .glue::DisposeDialog(iVar4);
  }
  .glue::SetPort(local_d0[0]);
  _BreakPointsToSoundFile(*(undefined8 *)(local_10c + 0x6f8),dVar21);
  return;
}




/* Função: .ClassicScoreFlowing | Endereço: 1000798c */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_ClassicScoreFlowing(void)

{
  float fVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 extraout_r4;
  undefined4 in_r6;
  undefined4 in_r7;
  ushort uVar4;
  short sVar5;
  double dVar6;
  double dVar7;
  double dVar8;
  double extraout_f1;
  double dVar9;
  double dVar10;
  double dVar11;
  double dVar12;
  double dVar13;
  double dVar14;
  double dVar15;
  double dVar16;
  double dVar17;
  double dVar18;
  double dVar19;
  undefined4 local_10c;
  undefined8 local_b8;
  
  dVar19 = (double)_DAT_1000c63c;
  DAT_1000d64c = '\x01';
  .glue::GetDateTime(0x1000da9c);
  if (DAT_1000d64c == '\0') {
    uVar4 = (ushort)(int)(dVar19 * (double)*(float *)(local_10c + 0x6d0));
    uVar3 = .glue::GetNewDialog(200,0,0xffffffff);
    .glue::SetPort(uVar3);
    .glue::DrawDialog(uVar3);
  }
  else {
    uVar4 = (ushort)(int)(dVar19 / (double)*(float *)(local_10c + 0x6b0));
  }
  dVar13 = *(double *)(local_10c + 0x770);
  dVar12 = *(double *)(local_10c + 0x6d8);
  dVar11 = *(double *)(local_10c + 0x768);
  dVar10 = *(double *)(local_10c + 0x760);
  dVar15 = *(double *)(local_10c + 0x758);
  dVar14 = *(double *)(local_10c + 0x750);
  dVar9 = *(double *)(local_10c + 0x748);
  for (sVar5 = 0; sVar5 <= (short)uVar4; sVar5 = sVar5 + 1) {
    dVar16 = (double)_FRand();
    dVar17 = (double)_FRand();
    iVar2 = (int)(short)(int)(dVar13 * dVar16);
    local_b8 = (double)CONCAT44(0x43300000,iVar2 * iVar2 ^ 0x80000000);
    _SetGF(local_b8 - dVar12,dVar19 * dVar17);
    dVar16 = (double)_FRand();
    dVar17 = (double)_FRand();
    iVar2 = (int)(short)(int)(dVar13 * dVar16);
    _SetGF((double)CONCAT44(0x43300000,iVar2 * iVar2 ^ 0x80000000) - dVar12,dVar19 * dVar17);
    dVar16 = (double)_FRand();
    dVar17 = (double)_FRand();
    _GFRnd(dVar11 * dVar17,dVar19 * dVar16);
    dVar16 = (double)_FRand();
    dVar17 = (double)_FRand();
    _SetGL(dVar19 * dVar16,(int)(dVar10 * dVar17));
    dVar16 = (double)_FRand();
    dVar17 = (double)_FRand();
    _GLRnd(dVar11 * dVar17,dVar19 * dVar16);
    dVar16 = (double)_FRand();
    dVar17 = (double)_FRand();
    _SetB(dVar15 * dVar17 - dVar14,dVar19 * dVar16);
    dVar16 = (double)_FRand();
    dVar17 = (double)_FRand();
    _BRnd(dVar9 * dVar17,dVar19 * dVar16);
  }
  if (DAT_1000d64c != '\0') {
    uVar4 = ((short)uVar4 >> 1) + (ushort)((short)uVar4 < 0 && (uVar4 & 1) != 0);
  }
  dVar10 = *(double *)(local_10c + 0x738);
  dVar11 = *(double *)(local_10c + 0x740);
  dVar12 = *(double *)(local_10c + 0x730);
  dVar13 = *(double *)(local_10c + 0x6d8);
  dVar14 = *(double *)(local_10c + 0x728);
  dVar15 = *(double *)(local_10c + 0x720);
  dVar16 = *(double *)(local_10c + 0x6c0);
  dVar17 = *(double *)(local_10c + 0x710);
  dVar18 = *(double *)(local_10c + 0x6c8);
  fVar1 = *(float *)(local_10c + 0x700);
  dVar9 = *(double *)(local_10c + 0x758);
  for (sVar5 = 0; sVar5 <= (short)uVar4; sVar5 = sVar5 + 1) {
    dVar6 = (double)_FRand();
    dVar7 = (double)_FRand();
    dVar8 = (double)_FRand();
    _SetSO((_DAT_1000d650 - dVar12) * dVar8,dVar19 * dVar7);
    dVar7 = (double)_FRand();
    dVar8 = (double)_FRand();
    iVar2 = (int)(short)(int)(dVar10 * dVar6 + dVar11);
    _SORnd((((double)CONCAT44(0x43300000,iVar2 * iVar2 ^ 0x80000000) - dVar13) * dVar8) / dVar14,
           dVar19 * dVar7);
    dVar6 = (double)_FRand();
    dVar6 = dVar15 * dVar6;
    dVar7 = (double)_FRand();
    dVar8 = (double)_FRand();
    uVar3 = _FRand();
    _SetFM(dVar17 * extraout_f1 + dVar18,dVar16 * dVar8 + dVar16,*(undefined8 *)(local_10c + 0x6f8),
           *(undefined8 *)(local_10c + 0x6f8),dVar19 * dVar7,uVar3,extraout_r4,(int)dVar6,in_r6,
           in_r7,(int)(fVar1 - (float)dVar6));
    dVar6 = (double)_FRand();
    dVar7 = (double)_FRand();
    _FMRnd(dVar9 * dVar7,dVar19 * dVar6);
  }
  _SetGF(*(undefined8 *)(local_10c + 0x768),*(undefined8 *)(local_10c + 0x6f8));
  _SetSO(*(undefined8 *)(local_10c + 0x6f0),*(undefined8 *)(local_10c + 0x6f8));
  _SetGL(*(undefined8 *)(local_10c + 0x6f8),200);
  _SetA(*(undefined8 *)(local_10c + 0x6f0),*(undefined8 *)(local_10c + 0x6f8));
  _ARnd(*(undefined8 *)(local_10c + 0x6f8),*(undefined8 *)(local_10c + 0x6f8));
  _SetD(*(undefined8 *)(local_10c + 0x6f0),*(undefined8 *)(local_10c + 0x6f8));
  _DRnd(*(undefined8 *)(local_10c + 0x6f8),*(undefined8 *)(local_10c + 0x6f8));
  _SetG(*(undefined8 *)(local_10c + 0x6b8),*(undefined8 *)(local_10c + 0x6e0));
  _GRnd(*(undefined8 *)(local_10c + 0x6f8),*(undefined8 *)(local_10c + 0x6f8));
  _BreakPointsToSoundFile(*(undefined8 *)(local_10c + 0x6f8),dVar19);
  return;
}




/* Função: .Slack | Endereço: 10007e9c */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_Slack(void)

{
  uint uVar1;
  float fVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 extraout_r4;
  undefined4 in_r6;
  undefined4 in_r7;
  ushort uVar5;
  short sVar6;
  double dVar7;
  double dVar8;
  double dVar9;
  double dVar10;
  double dVar11;
  double dVar12;
  double extraout_f1;
  double dVar13;
  double dVar14;
  double dVar15;
  double dVar16;
  double dVar17;
  double dVar18;
  double dVar19;
  double dVar20;
  undefined4 local_10c;
  undefined8 local_c8;
  undefined8 local_b8;
  undefined8 local_b0;
  
  dVar20 = (double)_DAT_1000c63c;
  DAT_1000d64c = '\x01';
  .glue::GetDateTime(0x1000da9c);
  if (DAT_1000d64c == '\0') {
    uVar5 = (ushort)(int)(dVar20 * (double)*(float *)(local_10c + 0x6a8));
    uVar4 = .glue::GetNewDialog(200,0,0xffffffff);
    .glue::SetPort(uVar4);
    .glue::DrawDialog(uVar4);
  }
  else {
    uVar5 = (ushort)(int)(dVar20 / (double)*(float *)(local_10c + 0x6ac));
  }
  dVar8 = (double)_FRand();
  dVar7 = *(double *)(local_10c + 0x770);
  dVar9 = (double)_FRand();
  iVar3 = (int)(short)(int)(dVar7 * dVar8);
  local_b8 = (double)CONCAT44(0x43300000,iVar3 * iVar3 ^ 0x80000000);
  _SetGF(local_b8 - *(double *)(local_10c + 0x6d8),*(double *)(local_10c + 0x6a0) * dVar9);
  dVar15 = *(double *)(local_10c + 0x770);
  dVar14 = *(double *)(local_10c + 0x6d8);
  dVar13 = *(double *)(local_10c + 0x768);
  dVar9 = *(double *)(local_10c + 0x760);
  dVar16 = *(double *)(local_10c + 0x758);
  dVar8 = *(double *)(local_10c + 0x750);
  dVar7 = *(double *)(local_10c + 0x748);
  for (sVar6 = 0; sVar6 <= (short)uVar5; sVar6 = sVar6 + 1) {
    dVar17 = (double)_FRand();
    dVar18 = (double)_FRand();
    iVar3 = (int)(short)(int)(dVar15 * dVar17);
    local_b0 = (double)CONCAT44(0x43300000,iVar3 * iVar3 ^ 0x80000000);
    _SetGF(local_b0 - dVar14,dVar20 * dVar18);
    dVar17 = (double)_FRand();
    dVar18 = (double)_FRand();
    iVar3 = (int)(short)(int)(dVar15 * dVar17);
    local_c8 = (double)CONCAT44(0x43300000,iVar3 * iVar3 ^ 0x80000000);
    _SetGF(local_c8 - dVar14,dVar20 * dVar18);
    dVar17 = (double)_FRand();
    dVar18 = (double)_FRand();
    _GFRnd(dVar13 * dVar18,dVar20 * dVar17);
    dVar17 = (double)_FRand();
    dVar18 = (double)_FRand();
    _SetGL(dVar20 * dVar17,(int)(dVar9 * dVar18));
    dVar17 = (double)_FRand();
    dVar18 = (double)_FRand();
    _GLRnd(dVar13 * dVar18,dVar20 * dVar17);
    dVar17 = (double)_FRand();
    dVar18 = (double)_FRand();
    _SetB(dVar16 * dVar18 - dVar8,dVar20 * dVar17);
    dVar17 = (double)_FRand();
    dVar18 = (double)_FRand();
    _BRnd(dVar7 * dVar18,dVar20 * dVar17);
  }
  if (DAT_1000d64c != '\0') {
    uVar5 = ((short)uVar5 >> 1) + (ushort)((short)uVar5 < 0 && (uVar5 & 1) != 0);
  }
  dVar8 = *(double *)(local_10c + 0x738);
  dVar9 = *(double *)(local_10c + 0x740);
  dVar13 = *(double *)(local_10c + 0x730);
  dVar14 = *(double *)(local_10c + 0x6d8);
  dVar15 = *(double *)(local_10c + 0x728);
  dVar16 = *(double *)(local_10c + 0x720);
  dVar17 = *(double *)(local_10c + 0x6c0);
  dVar18 = *(double *)(local_10c + 0x710);
  dVar19 = *(double *)(local_10c + 0x698);
  fVar2 = *(float *)(local_10c + 0x700);
  dVar7 = *(double *)(local_10c + 0x758);
  for (sVar6 = 0; sVar6 <= (short)uVar5; sVar6 = sVar6 + 1) {
    dVar10 = (double)_FRand();
    dVar11 = (double)_FRand();
    dVar12 = (double)_FRand();
    _SetSO((_DAT_1000d650 - dVar13) * dVar12,dVar20 * dVar11);
    dVar11 = (double)_FRand();
    dVar12 = (double)_FRand();
    iVar3 = (int)(short)(int)(dVar8 * dVar10 + dVar9);
    local_c8 = (double)CONCAT44(0x43300000,iVar3 * iVar3 ^ 0x80000000);
    _SORnd(((local_c8 - dVar14) * dVar12) / dVar15,dVar20 * dVar11);
    dVar10 = (double)_FRand();
    dVar11 = (double)_FRand();
    dVar11 = (double)(float)(dVar16 * dVar10) * dVar11;
    dVar10 = (double)(float)dVar11;
    uVar1 = (uint)dVar11;
    if (uVar1 != (((int)uVar1 >> 1) + (uint)((int)uVar1 < 0 && (uVar1 & 1) != 0)) * 2) {
      dVar10 = (double)(float)(dVar16 - dVar10);
    }
    dVar11 = (double)_FRand();
    dVar12 = (double)_FRand();
    uVar4 = _FRand();
    _SetFM(dVar18 * extraout_f1 + dVar19,dVar17 * dVar12 + dVar17,*(undefined8 *)(local_10c + 0x6f8)
           ,*(undefined8 *)(local_10c + 0x6f8),dVar20 * dVar11,uVar4,extraout_r4,(int)dVar10,in_r6,
           in_r7,(int)((double)fVar2 - dVar10));
    dVar10 = (double)_FRand();
    dVar11 = (double)_FRand();
    _FMRnd(dVar7 * dVar11,dVar20 * dVar10);
  }
  _SetGF(*(undefined8 *)(local_10c + 0x768),*(undefined8 *)(local_10c + 0x6f8));
  _SetSO(*(undefined8 *)(local_10c + 0x6f0),*(undefined8 *)(local_10c + 0x6f8));
  _SetGL(*(undefined8 *)(local_10c + 0x6f8),200);
  _SetA(*(undefined8 *)(local_10c + 0x6f0),*(undefined8 *)(local_10c + 0x6f8));
  _ARnd(*(undefined8 *)(local_10c + 0x6f8),*(undefined8 *)(local_10c + 0x6f8));
  _SetD(*(undefined8 *)(local_10c + 0x6f0),*(undefined8 *)(local_10c + 0x6f8));
  _DRnd(*(undefined8 *)(local_10c + 0x6f8),*(undefined8 *)(local_10c + 0x6f8));
  _SetG(*(undefined8 *)(local_10c + 0x6b8),*(undefined8 *)(local_10c + 0x6e0));
  _GRnd(*(undefined8 *)(local_10c + 0x6f8),*(undefined8 *)(local_10c + 0x6f8));
  _BreakPointsToSoundFile(*(undefined8 *)(local_10c + 0x6f8),dVar20);
  return;
}




/* Função: .Shepard | Endereço: 1000842c */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_Shepard(void)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  short sVar6;
  short sVar7;
  short sVar8;
  int iVar5;
  undefined4 in_r6;
  undefined4 in_r7;
  double dVar9;
  double dVar10;
  double dVar11;
  double dVar12;
  double dVar13;
  double dVar14;
  double dVar15;
  double dVar16;
  double dVar17;
  double dVar18;
  double dVar19;
  double dVar20;
  double dVar21;
  double dVar22;
  double dVar23;
  double dVar24;
  double dVar25;
  undefined6 uVar26;
  undefined8 uVar27;
  int local_14c;
  
  dVar23 = (double)_DAT_1000c63c;
  .glue::GetDateTime(0x1000da9c);
  _SetGF(*(undefined8 *)(local_14c + 0x688),*(undefined8 *)(local_14c + 0x6f8));
  _GFRnd(*(undefined8 *)(local_14c + 0x720),*(undefined8 *)(local_14c + 0x6f8));
  _SetGL(*(undefined8 *)(local_14c + 0x6f8),2000);
  _GLRnd(*(undefined8 *)(local_14c + 0x6f8),*(undefined8 *)(local_14c + 0x6f8));
  _SetA(*(undefined8 *)(local_14c + 0x6f0),*(undefined8 *)(local_14c + 0x6f8));
  _ARnd(*(undefined8 *)(local_14c + 0x6f8),*(undefined8 *)(local_14c + 0x6f8));
  _SetD(*(undefined8 *)(local_14c + 0x6f0),*(undefined8 *)(local_14c + 0x6f8));
  _DRnd(*(undefined8 *)(local_14c + 0x6f8),*(undefined8 *)(local_14c + 0x6f8));
  _SetG(*(undefined8 *)(local_14c + 0x6f0),*(undefined8 *)(local_14c + 0x6f8));
  uVar27 = _GRnd(*(undefined8 *)(local_14c + 0x6f8),*(undefined8 *)(local_14c + 0x6f8));
  uVar27 = _SetFM(*(undefined8 *)(local_14c + 0x680),*(undefined8 *)(local_14c + 0x678),
                  *(undefined8 *)(local_14c + 0x670),*(undefined8 *)(local_14c + 0x680),
                  *(undefined8 *)(local_14c + 0x6f8),(int)((ulonglong)uVar27 >> 0x20),(int)uVar27,
                  0x19,in_r6,in_r7,0x19);
  uVar27 = _SetFM(*(undefined8 *)(local_14c + 0x6f8),*(undefined8 *)(local_14c + 0x678),
                  *(undefined8 *)(local_14c + 0x670),*(undefined8 *)(local_14c + 0x680),
                  *(undefined8 *)(local_14c + 0x668),(int)((ulonglong)uVar27 >> 0x20),(int)uVar27,
                  0x19,in_r6,in_r7,0x19);
  _SetFM(*(undefined8 *)(local_14c + 0x698),*(undefined8 *)(local_14c + 0x6f8),
         *(undefined8 *)(local_14c + 0x670),*(undefined8 *)(local_14c + 0x680),
         *(undefined8 *)(local_14c + 0x660),(int)((ulonglong)uVar27 >> 0x20),(int)uVar27,0x19,in_r6,
         in_r7,0x19);
  fVar1 = *(float *)(local_14c + 0x658);
  fVar2 = *(float *)(local_14c + 0x654);
  dVar19 = (double)*(float *)(local_14c + 0x650);
  fVar3 = *(float *)(local_14c + 0x64c);
  fVar4 = *(float *)(local_14c + 0x6b0);
  dVar9 = *(double *)(local_14c + 0x640);
  dVar24 = *(double *)(local_14c + 0x6d8);
  dVar25 = (double)*(float *)(local_14c + 0x648);
  dVar10 = *(double *)(local_14c + 0x6f0);
  dVar11 = *(double *)(local_14c + 0x750);
  while( true ) {
    dVar22 = (double)fVar3;
    dVar21 = (double)fVar1;
    dVar20 = (double)fVar2;
    dVar12 = (double)fVar4;
    if (dVar23 <= dVar12) break;
    sVar6 = _Rand(8);
    dVar18 = (double)(float)((double)CONCAT44(0x43300000,(int)sVar6 ^ 0x80000000) - dVar24);
    sVar6 = _Rand(8);
    dVar17 = (double)(float)((double)CONCAT44(0x43300000,(int)sVar6 ^ 0x80000000) - dVar24);
    sVar6 = _Rand(8);
    dVar16 = (double)(float)((double)CONCAT44(0x43300000,(int)sVar6 ^ 0x80000000) - dVar24);
    sVar6 = _Rand(8);
    dVar15 = (double)(float)((double)CONCAT44(0x43300000,(int)sVar6 ^ 0x80000000) - dVar24);
    sVar6 = _Rand(6);
    sVar7 = _Rand(6);
    sVar8 = _Rand(6);
    uVar27 = _Rand(6);
    dVar13 = (double)(float)((double)CONCAT44(0x43300000,
                                              (int)(short)((ulonglong)uVar27 >> 0x20) ^ 0x80000000)
                            - dVar24);
    uVar27 = _SetFM((double)(float)(dVar21 + dVar18),(double)(float)(dVar21 + dVar17),dVar19,
                    (double)(float)(dVar20 + dVar13),dVar12,(int)((ulonglong)uVar27 >> 0x20),
                    (int)uVar27,0,in_r6,in_r7,0x1e);
    dVar14 = (double)(float)(dVar20 + (double)(float)((double)CONCAT44(0x43300000,
                                                                       (int)sVar6 ^ 0x80000000) -
                                                     dVar24));
    _SetFM((double)(float)(dVar25 + dVar14),(double)(float)(dVar21 + dVar17),dVar19,
           (double)(float)(dVar20 + dVar13),dVar10 + dVar12,(int)((ulonglong)uVar27 >> 0x20),
           (int)uVar27,0,in_r6,in_r7,0x1e);
    uVar26 = _Rand(3);
    iVar5 = (int)(short)((uint6)uVar26 >> 0x20);
    dVar12 = (double)(float)(dVar22 * (double)(float)((double)CONCAT44(0x43300000,
                                                                       iVar5 + 3U ^ 0x80000000) -
                                                     dVar24) + dVar12);
    uVar27 = _SetFM(dVar14,(double)(float)(dVar21 + dVar17),(double)(float)(dVar21 + dVar16),dVar19,
                    dVar12,iVar5,(int)uVar26,0x1e,in_r6,in_r7,0);
    dVar17 = (double)(float)(dVar20 + (double)(float)((double)CONCAT44(0x43300000,
                                                                       (int)sVar7 ^ 0x80000000) -
                                                     dVar24));
    _SetFM(dVar14,(double)(float)(dVar25 + dVar17),(double)(float)(dVar21 + dVar16),dVar19,
           dVar10 + dVar12,(int)((ulonglong)uVar27 >> 0x20),(int)uVar27,0x1e,in_r6,in_r7,0);
    uVar26 = _Rand(3);
    iVar5 = (int)(short)((uint6)uVar26 >> 0x20);
    dVar12 = (double)(float)(dVar22 * (double)(float)((double)CONCAT44(0x43300000,
                                                                       iVar5 + 3U ^ 0x80000000) -
                                                     dVar24) + dVar12);
    uVar27 = _SetFM(dVar19,dVar17,(double)(float)(dVar21 + dVar16),(double)(float)(dVar21 + dVar15),
                    dVar12,iVar5,(int)uVar26,0x1e,in_r6,in_r7,0x1e);
    dVar14 = (double)(float)(dVar20 + (double)(float)((double)CONCAT44(0x43300000,
                                                                       (int)sVar8 ^ 0x80000000) -
                                                     dVar24));
    _SetFM(dVar19,dVar17,(double)(float)(dVar25 + dVar14),(double)(float)(dVar21 + dVar15),
           dVar10 + dVar12,(int)((ulonglong)uVar27 >> 0x20),(int)uVar27,0x1e,in_r6,in_r7,0x1e);
    sVar6 = _Rand(3);
    dVar12 = (double)(float)(dVar22 * (double)(float)((double)CONCAT44(0x43300000,
                                                                       (int)sVar6 + 3U ^ 0x80000000)
                                                     - dVar24) + dVar12);
    _SetFM((double)(float)(dVar21 + dVar18),dVar19,dVar14,(double)(float)(dVar21 + dVar15),dVar12);
    _SetFM((double)(float)(dVar21 + dVar18),dVar19,dVar14,
           (double)(float)(dVar25 + (double)(float)(dVar20 + dVar13)),dVar10 + dVar12);
    sVar6 = _Rand(3);
    fVar1 = (float)(dVar21 - dVar11);
    fVar2 = (float)(dVar20 - dVar11);
    fVar4 = (float)(dVar22 * (double)(float)((double)CONCAT44(0x43300000,
                                                              (int)sVar6 + 3U ^ 0x80000000) - dVar24
                                            ) + dVar12);
    fVar3 = (float)(dVar9 + dVar22);
  }
  _FMRnd(*(undefined8 *)(local_14c + 0x638),*(undefined8 *)(local_14c + 0x6f8));
  _SetB(*(undefined8 *)(local_14c + 0x6f8),*(undefined8 *)(local_14c + 0x6f8));
  _BRnd(*(undefined8 *)(local_14c + 0x630),*(undefined8 *)(local_14c + 0x6f8));
  _SetSO(*(undefined8 *)(local_14c + 0x628),*(undefined8 *)(local_14c + 0x6f8));
  _SetSO(*(double *)(local_14c + 0x1790) - *(double *)(local_14c + 0x628),dVar23);
  _SORnd(*(undefined8 *)(local_14c + 0x6f0),*(undefined8 *)(local_14c + 0x6f8));
  _BreakPointsToSoundFile(*(undefined8 *)(local_14c + 0x6f8),dVar23);
  return;
}




/* Função: .PentaScore | Endereço: 10008a78 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_PentaScore(void)

{
  bool bVar1;
  float fVar2;
  float fVar3;
  undefined **ppuVar4;
  undefined4 *puVar5;
  undefined *puVar6;
  short sVar7;
  short sVar8;
  short sVar9;
  undefined **ppuVar10;
  undefined4 *puVar11;
  short sVar12;
  short sVar13;
  int iVar14;
  double dVar15;
  double dVar16;
  double dVar17;
  double dVar18;
  double dVar19;
  int local_fc;
  undefined4 auStack_c8 [2];
  float local_c0 [26];
  longlong local_58;
  
  iVar14 = 0xb;
  dVar18 = (double)_DAT_1000c63c;
  sVar12 = 4;
  ppuVar4 = &PTR_LAB_1000cd64;
  puVar5 = auStack_c8;
  do {
    puVar11 = puVar5;
    ppuVar10 = ppuVar4;
    puVar6 = ppuVar10[3];
    puVar11[2] = ppuVar10[2];
    puVar11[3] = puVar6;
    iVar14 = iVar14 + -1;
    ppuVar4 = ppuVar10 + 2;
    puVar5 = puVar11 + 2;
  } while (iVar14 != 0);
  puVar11[4] = ppuVar10[4];
  .glue::GetDateTime(0x1000da9c);
  _SetGF(*(undefined8 *)(local_fc + 0x688),*(undefined8 *)(local_fc + 0x6f8));
  _GFRnd(*(undefined8 *)(local_fc + 0x720),*(undefined8 *)(local_fc + 0x6f8));
  _SetGL(*(undefined8 *)(local_fc + 0x6f8),2000);
  _GLRnd(*(undefined8 *)(local_fc + 0x6f8),*(undefined8 *)(local_fc + 0x6f8));
  _SetA(*(undefined8 *)(local_fc + 0x628),*(undefined8 *)(local_fc + 0x6f8));
  _ARnd(*(undefined8 *)(local_fc + 0x6f8),*(undefined8 *)(local_fc + 0x6f8));
  _SetD(*(undefined8 *)(local_fc + 0x628),*(undefined8 *)(local_fc + 0x6f8));
  _DRnd(*(undefined8 *)(local_fc + 0x6f8),*(undefined8 *)(local_fc + 0x6f8));
  _SetG(*(undefined8 *)(local_fc + 0x730),*(undefined8 *)(local_fc + 0x6f8));
  _GRnd(*(undefined8 *)(local_fc + 0x6f8),*(undefined8 *)(local_fc + 0x6f8));
  fVar2 = *(float *)(local_fc + 0x624);
  dVar19 = (double)*(float *)(local_fc + 0x690);
  iVar14 = (int)(dVar18 * (*(double *)(local_fc + 0x638) / (double)fVar2));
  local_58 = (longlong)iVar14;
  sVar7 = _Rand(10);
  sVar7 = sVar7 + 5;
  sVar8 = _Rand(5);
  fVar3 = *(float *)(local_fc + 0x620);
  sVar8 = sVar8 + 3;
  dVar15 = *(double *)(local_fc + 0x618);
  for (sVar13 = 0; sVar13 <= (short)iVar14; sVar13 = sVar13 + 1) {
    dVar16 = (double)_FRand();
    dVar19 = (double)(float)((double)fVar3 + dVar15 * (double)fVar2 * dVar16 + dVar19);
    _SetFM((double)local_c0[sVar7],(double)local_c0[sVar8],*(undefined8 *)(local_fc + 0x6f8),
           *(undefined8 *)(local_fc + 0x6f8),dVar19);
    sVar9 = _Rand(5);
    sVar7 = sVar9 + sVar7 + -3;
    if ((sVar7 < 6) || (0x13 < sVar7)) {
      sVar7 = _Rand(0xf);
      sVar7 = sVar7 + 5;
    }
    bVar1 = sVar12 == 0;
    sVar12 = sVar12 + -1;
    if (bVar1) {
      sVar12 = 2;
      sVar9 = _Rand(5);
      sVar8 = sVar9 + sVar8 + -3;
      if ((sVar8 < 0) || (0xc < sVar8)) {
        sVar8 = _Rand(10);
      }
      dVar16 = (double)_FRand();
      dVar17 = (double)_FRand();
      _SetSO(_DAT_1000d650 * dVar17,dVar18 * dVar16);
    }
    _SetFM((double)local_c0[sVar7],(double)local_c0[sVar8],*(undefined8 *)(local_fc + 0x6f8),
           *(undefined8 *)(local_fc + 0x6f8),(double)(float)(dVar19 + (double)fVar3));
  }
  _SORnd(*(undefined8 *)(local_fc + 0x6f0),*(undefined8 *)(local_fc + 0x6f8));
  _FMRnd(*(undefined8 *)(local_fc + 0x6f8),*(undefined8 *)(local_fc + 0x6f8));
  _SetB(*(undefined8 *)(local_fc + 0x6f8),*(undefined8 *)(local_fc + 0x6f8));
  _BRnd(*(undefined8 *)(local_fc + 0x748),*(undefined8 *)(local_fc + 0x6f8));
  _BreakPointsToSoundFile(*(undefined8 *)(local_fc + 0x6f8),dVar18);
  return;
}




/* Função: .stretch1Score | Endereço: 10008ddc */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_stretch1Score(void)

{
  float fVar1;
  float fVar2;
  short sVar4;
  uint uVar3;
  undefined4 extraout_r4;
  undefined4 in_r6;
  undefined4 in_r7;
  short sVar5;
  double dVar6;
  double dVar7;
  double dVar8;
  undefined4 local_8c;
  undefined8 local_38;
  
  dVar7 = (double)_DAT_1000c4d4;
  .glue::GetDateTime(0x1000da9c);
  fVar1 = *(float *)(local_8c + 0x610);
  _SetGF(*(undefined8 *)(local_8c + 0x608),*(undefined8 *)(local_8c + 0x6f8));
  _GFRnd(*(undefined8 *)(local_8c + 0x720),*(undefined8 *)(local_8c + 0x6f8));
  _SetGL(*(undefined8 *)(local_8c + 0x6f8),3000);
  _GLRnd(*(undefined8 *)(local_8c + 0x720),*(undefined8 *)(local_8c + 0x6f8));
  _SetA(*(undefined8 *)(local_8c + 0x730),*(undefined8 *)(local_8c + 0x6f8));
  _ARnd(*(undefined8 *)(local_8c + 0x6f8),*(undefined8 *)(local_8c + 0x6f8));
  _SetD(*(undefined8 *)(local_8c + 0x730),*(undefined8 *)(local_8c + 0x6f8));
  _DRnd(*(undefined8 *)(local_8c + 0x6f8),*(undefined8 *)(local_8c + 0x6f8));
  _SetG(*(undefined8 *)(local_8c + 0x730),*(undefined8 *)(local_8c + 0x6f8));
  _GRnd(*(undefined8 *)(local_8c + 0x6f8),*(undefined8 *)(local_8c + 0x6f8));
  fVar2 = *(float *)(local_8c + 0x604);
  dVar8 = *(double *)(local_8c + 0x6d8);
  for (sVar5 = 0; sVar5 < (short)(int)(dVar7 * (double)fVar1); sVar5 = sVar5 + 1) {
    sVar4 = _Rand(0x1e);
    uVar3 = (uint)sVar4;
    if (uVar3 != (((int)uVar3 >> 1) + (uint)((int)uVar3 < 0 && (uVar3 & 1) != 0)) * 2) {
      sVar4 = 0x3c - sVar4;
    }
    dVar6 = (double)_FRand();
    local_38 = (double)CONCAT44(0x43300000,(int)sVar4 ^ 0x80000000U);
    _SetFM(*(undefined8 *)(local_8c + 0x6f8),*(undefined8 *)(local_8c + 0x698),
           *(undefined8 *)(local_8c + 0x6c0),*(undefined8 *)(local_8c + 0x6f8),dVar7 * dVar6,
           (int)(fVar2 - (float)(local_38 - dVar8)),extraout_r4,0x28,in_r6,in_r7,
           (int)((double)CONCAT44(0x43300000,(int)sVar4 ^ 0x80000000U) - dVar8));
    _FMRnd(*(undefined8 *)(local_8c + 0x6f8),*(undefined8 *)(local_8c + 0x6f8));
  }
  _SetB(*(undefined8 *)(local_8c + 0x6f8),*(undefined8 *)(local_8c + 0x6f8));
  _BRnd(*(undefined8 *)(local_8c + 0x668),*(undefined8 *)(local_8c + 0x6f8));
  _SetSO(*(undefined8 *)(local_8c + 0x6f8),*(undefined8 *)(local_8c + 0x6f8));
  _SetSO(*(undefined8 *)(local_8c + 0x1790),dVar7);
  _SORnd(*(undefined8 *)(local_8c + 0x6f8),*(undefined8 *)(local_8c + 0x6f8));
  _BreakPointsToSoundFile(*(undefined8 *)(local_8c + 0x6f8),dVar7);
  return;
}




/* Função: .stretch5Score | Endereço: 10009068 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_stretch5Score(void)

{
  float fVar1;
  float fVar2;
  short sVar4;
  uint uVar3;
  undefined4 extraout_r4;
  undefined4 in_r6;
  undefined4 in_r7;
  short sVar5;
  double dVar6;
  double dVar7;
  double dVar8;
  undefined4 local_8c;
  undefined8 local_38;
  
  dVar7 = (double)_DAT_1000c4c0;
  .glue::GetDateTime(0x1000da9c);
  fVar1 = *(float *)(local_8c + 0x610);
  _SetGF(*(undefined8 *)(local_8c + 0x608),*(undefined8 *)(local_8c + 0x6f8));
  _GFRnd(*(undefined8 *)(local_8c + 0x720),*(undefined8 *)(local_8c + 0x6f8));
  _SetGL(*(undefined8 *)(local_8c + 0x6f8),3000);
  _GLRnd(*(undefined8 *)(local_8c + 0x720),*(undefined8 *)(local_8c + 0x6f8));
  _SetA(*(undefined8 *)(local_8c + 0x730),*(undefined8 *)(local_8c + 0x6f8));
  _ARnd(*(undefined8 *)(local_8c + 0x6f8),*(undefined8 *)(local_8c + 0x6f8));
  _SetD(*(undefined8 *)(local_8c + 0x730),*(undefined8 *)(local_8c + 0x6f8));
  _DRnd(*(undefined8 *)(local_8c + 0x6f8),*(undefined8 *)(local_8c + 0x6f8));
  _SetG(*(undefined8 *)(local_8c + 0x730),*(undefined8 *)(local_8c + 0x6f8));
  _GRnd(*(undefined8 *)(local_8c + 0x6f8),*(undefined8 *)(local_8c + 0x6f8));
  fVar2 = *(float *)(local_8c + 0x604);
  dVar8 = *(double *)(local_8c + 0x6d8);
  for (sVar5 = 0; sVar5 < (short)(int)(dVar7 * (double)fVar1); sVar5 = sVar5 + 1) {
    sVar4 = _Rand(0x1e);
    uVar3 = (uint)sVar4;
    if (uVar3 != (((int)uVar3 >> 1) + (uint)((int)uVar3 < 0 && (uVar3 & 1) != 0)) * 2) {
      sVar4 = 0x3c - sVar4;
    }
    dVar6 = (double)_FRand();
    local_38 = (double)CONCAT44(0x43300000,(int)sVar4 ^ 0x80000000U);
    _SetFM(*(undefined8 *)(local_8c + 0x6f8),*(undefined8 *)(local_8c + 0x698),
           *(undefined8 *)(local_8c + 0x6c0),*(undefined8 *)(local_8c + 0x6f8),dVar7 * dVar6,
           (int)(fVar2 - (float)(local_38 - dVar8)),extraout_r4,0x28,in_r6,in_r7,
           (int)((double)CONCAT44(0x43300000,(int)sVar4 ^ 0x80000000U) - dVar8));
    _FMRnd(*(undefined8 *)(local_8c + 0x6f8),*(undefined8 *)(local_8c + 0x6f8));
  }
  _SetB(*(undefined8 *)(local_8c + 0x6f8),*(undefined8 *)(local_8c + 0x6f8));
  _BRnd(*(undefined8 *)(local_8c + 0x5f8),*(undefined8 *)(local_8c + 0x6f8));
  _SetSO(*(undefined8 *)(local_8c + 0x6f8),*(undefined8 *)(local_8c + 0x6f8));
  _SetSO(*(undefined8 *)(local_8c + 0x1790),dVar7);
  _SORnd(*(undefined8 *)(local_8c + 0x6f8),*(undefined8 *)(local_8c + 0x6f8));
  _BreakPointsToSoundFile(*(undefined8 *)(local_8c + 0x6f8),dVar7);
  return;
}




/* Função: .plainStretch1 | Endereço: 100092f0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_plainStretch1(void)

{
  undefined4 uVar1;
  undefined4 extraout_r4;
  undefined4 in_r6;
  undefined4 in_r7;
  double extraout_f1;
  undefined8 uVar2;
  double dVar3;
  undefined4 local_5c;
  
  dVar3 = (double)_DAT_1000c4d4;
  .glue::GetDateTime(0x1000da9c);
  _SetGF(*(undefined8 *)(local_5c + 0x5f0),*(undefined8 *)(local_5c + 0x6f8));
  _GFRnd(*(undefined8 *)(local_5c + 0x720),*(undefined8 *)(local_5c + 0x6f8));
  _SetGL(*(undefined8 *)(local_5c + 0x6f8),2000);
  _GLRnd(*(undefined8 *)(local_5c + 0x720),*(undefined8 *)(local_5c + 0x6f8));
  _SetA(*(undefined8 *)(local_5c + 0x730),*(undefined8 *)(local_5c + 0x6f8));
  _ARnd(*(undefined8 *)(local_5c + 0x6f8),*(undefined8 *)(local_5c + 0x6f8));
  _SetD(*(undefined8 *)(local_5c + 0x730),*(undefined8 *)(local_5c + 0x6f8));
  _DRnd(*(undefined8 *)(local_5c + 0x6f8),*(undefined8 *)(local_5c + 0x6f8));
  _SetG(*(undefined8 *)(local_5c + 0x5e8),*(undefined8 *)(local_5c + 0x6f8));
  _GRnd(*(undefined8 *)(local_5c + 0x6f8),*(undefined8 *)(local_5c + 0x6f8));
  uVar1 = _FRand();
  uVar2 = *(undefined8 *)(local_5c + 0x6f8);
  _SetFM(uVar2,uVar2,uVar2,uVar2,dVar3 * extraout_f1,uVar1,extraout_r4,100,in_r6,in_r7,0);
  _FMRnd(*(undefined8 *)(local_5c + 0x6f8),*(undefined8 *)(local_5c + 0x6f8));
  _SetB(*(undefined8 *)(local_5c + 0x6f8),*(undefined8 *)(local_5c + 0x6f8));
  _BRnd(*(undefined8 *)(local_5c + 0x5f8),*(undefined8 *)(local_5c + 0x6f8));
  _SetSO(*(undefined8 *)(local_5c + 0x6f8),*(undefined8 *)(local_5c + 0x6f8));
  _SetSO(*(undefined8 *)(local_5c + 0x1790),dVar3);
  _SORnd(*(undefined8 *)(local_5c + 0x6f8),*(undefined8 *)(local_5c + 0x6f8));
  _BreakPointsToSoundFile(*(undefined8 *)(local_5c + 0x6f8),dVar3);
  return;
}




/* Função: .plainStretch5 | Endereço: 10009494 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void .debug::_plainStretch5(void)

{
  undefined4 uVar1;
  undefined4 extraout_r4;
  undefined4 in_r6;
  undefined4 in_r7;
  double extraout_f1;
  undefined8 uVar2;
  double dVar3;
  undefined4 local_5c;
  
  dVar3 = (double)_DAT_1000c4c0;
  .glue::GetDateTime(0x1000da9c);
  _SetGF(*(undefined8 *)(local_5c + 0x5f0),*(undefined8 *)(local_5c + 0x6f8));
  _GFRnd(*(undefined8 *)(local_5c + 0x720),*(undefined8 *)(local_5c + 0x6f8));
  _SetGL(*(undefined8 *)(local_5c + 0x6f8),2000);
  _GLRnd(*(undefined8 *)(local_5c + 0x720),*(undefined8 *)(local_5c + 0x6f8));
  _SetA(*(undefined8 *)(local_5c + 0x730),*(undefined8 *)(local_5c + 0x6f8));
  _ARnd(*(undefined8 *)(local_5c + 0x6f8),*(undefined8 *)(local_5c + 0x6f8));
  _SetD(*(undefined8 *)(local_5c + 0x730),*(undefined8 *)(local_5c + 0x6f8));
  _DRnd(*(undefined8 *)(local_5c + 0x6f8),*(undefined8 *)(local_5c + 0x6f8));
  _SetG(*(undefined8 *)(local_5c + 0x5e8),*(undefined8 *)(local_5c + 0x6f8));
  _GRnd(*(undefined8 *)(local_5c + 0x6f8),*(undefined8 *)(local_5c + 0x6f8));
  uVar1 = _FRand();
  uVar2 = *(undefined8 *)(local_5c + 0x6f8);
  _SetFM(uVar2,uVar2,uVar2,uVar2,dVar3 * extraout_f1,uVar1,extraout_r4,100,in_r6,in_r7,0);
  _FMRnd(*(undefined8 *)(local_5c + 0x6f8),*(undefined8 *)(local_5c + 0x6f8));
  _SetB(*(undefined8 *)(local_5c + 0x6f8),*(undefined8 *)(local_5c + 0x6f8));
  _BRnd(*(undefined8 *)(local_5c + 0x5f8),*(undefined8 *)(local_5c + 0x6f8));
  _SetSO(*(undefined8 *)(local_5c + 0x6f8),*(undefined8 *)(local_5c + 0x6f8));
  _SetSO(*(undefined8 *)(local_5c + 0x1790),dVar3);
  _SORnd(*(undefined8 *)(local_5c + 0x6f8),*(undefined8 *)(local_5c + 0x6f8));
  _BreakPointsToSoundFile(*(undefined8 *)(local_5c + 0x6f8),dVar3);
  return;
}




/* Função: FUN_10009638 | Endereço: 10009638 */

int FUN_10009638(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                undefined4 param_5,undefined4 param_6,undefined4 param_7)

{
  undefined4 *puVar1;
  int iVar2;
  int iVar3;
  
  puVar1 = (undefined4 *)&DAT_1000d6cc;
  iVar2 = 0;
  iVar3 = 0x20;
  do {
    if (puVar1[2] == 0) {
      puVar1[2] = param_1;
      puVar1[3] = param_2;
      puVar1[4] = param_3;
      puVar1[5] = param_4;
      *puVar1 = param_5;
      puVar1[1] = param_6;
      puVar1[6] = param_7;
      return iVar2;
    }
    iVar2 = iVar2 + 1;
    puVar1 = puVar1 + 7;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  return -1;
}




/* Função: FUN_1000968c | Endereço: 1000968c */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_1000968c(void)

{
  undefined4 *puVar1;
  
  while (_DAT_1000da4c != (int *)0x0) {
    puVar1 = _DAT_1000da4c + 2;
    _DAT_1000da4c = (int *)*_DAT_1000da4c;
    FUN_1000bdec(*puVar1,0xffffffff);
  }
  return;
}




/* Função: FUN_100096e0 | Endereço: 100096e0 */

void FUN_100096e0(void)

{
  return;
}




/* Função: FUN_100096ec | Endereço: 100096ec */

undefined ** FUN_100096ec(void)

{
  return &.TVect::InitGraf;
}




/* Função: entry | Endereço: 100096f4 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void entry(void)

{
  undefined4 uVar1;
  undefined4 local_18 [5];
  
  local_18[0] = 0;
  FUN_100096e0();
  uVar1 = FUN_100096ec();
  _DAT_1000c640 =
       FUN_10009638(PTR_FUN_1000c164,_DAT_1000c160,&.TVect::InitGraf,&DAT_1000dc44,PTR_DAT_1000c154,
                    _DAT_1000c150,uVar1);
  FUN_10000000();
  .debug::_main(0,local_18);
  FUN_100097c4(0);
  return;
}




/* Função: FUN_10009774 | Endereço: 10009774 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int FUN_10009774(double param_1)

{
  bool bVar1;
  double dVar2;
  
  dVar2 = _DAT_1000c250[2];
  if (param_1 < *_DAT_1000c250) {
    return 0;
  }
  if (_DAT_1000c250[1] <= param_1) {
    return -1;
  }
  bVar1 = dVar2 <= param_1;
  if (bVar1) {
    param_1 = param_1 - dVar2;
  }
  if (!bVar1) {
    return (int)param_1;
  }
  return (int)param_1 + -0x80000000;
}




/* Função: FUN_100097c4 | Endereço: 100097c4 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_100097c4(undefined4 param_1)

{
  if (_DAT_1000dbac == 0) {
    while (_DAT_1000cde0 < 0x22) {
      _DAT_1000cde0 = _DAT_1000cde0 + 1;
      FUN_1000bdec();
    }
  }
  FUN_10009850(param_1);
  return;
}




/* Função: FUN_10009850 | Endereço: 10009850 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_10009850(void)

{
  FUN_1000968c();
  while (_DAT_1000cde4 < 0x22) {
    _DAT_1000cde4 = _DAT_1000cde4 + 1;
    FUN_1000bdec();
  }
  if (_DAT_1000dba8 != 0) {
    FUN_1000bdec();
  }
  .glue::ExitToShell();
  return;
}




/* Função: FUN_100098e8 | Endereço: 100098e8 */

int FUN_100098e8(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_1000b3b4(param_1,param_2,param_3);
  return iVar1 + param_3;
}




/* Função: FUN_10009920 | Endereço: 10009920 */

void FUN_10009920(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
                 undefined4 param_5,undefined4 param_6,undefined4 param_7,undefined4 param_8)

{
  int iVar1;
  undefined4 uStack0000001c;
  undefined4 uStack00000020;
  undefined4 uStack00000024;
  undefined4 uStack00000028;
  undefined4 uStack0000002c;
  undefined4 uStack00000030;
  undefined4 uStack00000034;
  
  uStack0000001c = param_2;
  uStack00000020 = param_3;
  uStack00000024 = param_4;
  uStack00000028 = param_5;
  uStack0000002c = param_6;
  uStack00000030 = param_7;
  uStack00000034 = param_8;
  iVar1 = FUN_1000a030(PTR_PTR_1000c254,param_1,param_2,&stack0x00000020);
  if (-1 < iVar1) {
    *(undefined1 *)(param_1 + iVar1) = 0;
  }
  return;
}




/* Função: FUN_1000998c | Endereço: 1000998c */

void FUN_1000998c(undefined8 *param_1,undefined4 param_2,undefined4 param_3)

{
  bool bVar1;
  int iVar2;
  byte bVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  char cVar8;
  int iVar9;
  byte *pbVar10;
  byte *pbVar11;
  undefined8 uVar12;
  byte local_158 [256];
  undefined1 local_58 [42];
  char local_2e [2];
  short local_2c;
  
  uVar12 = *param_1;
  pbVar10 = *(byte **)(param_1 + 2);
  iVar9 = 0;
  bVar1 = false;
  cVar8 = (char)param_2;
  if (*(int *)((int)param_1 + 0x2c) < 0) {
    *(undefined4 *)((int)param_1 + 0x2c) = 6;
  }
  else if ((*(int *)((int)param_1 + 0x2c) == 0) && ((cVar8 == 'g' || (cVar8 == 'G')))) {
    *(undefined4 *)((int)param_1 + 0x2c) = 1;
  }
  if (0x13 < *(int *)((int)param_1 + 0x2c)) {
    *(undefined4 *)((int)param_1 + 0x2c) = 0x13;
  }
  local_2c = (short)*(undefined4 *)((int)param_1 + 0x2c);
  if (cVar8 == 'e') {
LAB_10009a5c:
    local_2c = local_2c + 1;
  }
  else if (cVar8 < 'e') {
    if (cVar8 == 'F') goto LAB_10009a7c;
    if (cVar8 < 'F') {
      if (cVar8 < 'E') goto LAB_10009a7c;
      goto LAB_10009a5c;
    }
    if ('G' < cVar8) goto LAB_10009a7c;
  }
  else if (cVar8 != 'g') {
    if (cVar8 < 'g') {
      local_2e[0] = '\x01';
    }
    goto LAB_10009a7c;
  }
  local_2e[0] = '\0';
LAB_10009a7c:
  .glue::num2dec(uVar12,local_2e,param_2,param_3,local_58);
  local_58[0] = 0;
  .glue::dec2str(local_2e,local_58,local_158 + 1);
  local_158[0] = FUN_1000b424(local_158 + 1);
  if ((cVar8 == 'g') || (cVar8 == 'G')) {
    iVar6 = 0;
    iVar7 = 1;
    for (pbVar11 = local_158 + local_158[0];
        ((*pbVar11 != 0x65 && (*pbVar11 != 0x2b)) && (*pbVar11 != 0x2d)); pbVar11 = pbVar11 + -1) {
      iVar2 = ((char)*pbVar11 + -0x30) * iVar7;
      iVar7 = iVar7 * 10;
      iVar6 = iVar6 * 2 + iVar2;
    }
    if (*pbVar11 == 0x2d) {
      iVar6 = -iVar6;
    }
    if ((-5 < iVar6) && (iVar6 < *(int *)((int)param_1 + 0x2c))) {
      local_2e[0] = '\x01';
      iVar6 = *(int *)((int)param_1 + 0x2c) - iVar6;
      local_2c = (short)iVar6 + -1;
      .glue::num2dec(uVar12,local_2e,iVar6,iVar7,local_58);
      local_58[0] = 0;
      .glue::dec2str(local_2e,local_58,local_158 + 1);
      local_158[0] = FUN_1000b424(local_158 + 1);
    }
  }
  uVar5 = (uint)(local_158[1] == ' ');
  while (uVar4 = uVar5 + 1, (int)uVar4 <= (int)(uint)local_158[0]) {
    if (local_158[uVar5 + 1] == 0x65) {
      if (((*(ushort *)(param_1 + 7) & 8) == 0) && ((cVar8 == 'G' || (cVar8 == 'g')))) {
        for (; pbVar10[-1] == 0x30; pbVar10 = pbVar10 + -1) {
          iVar9 = iVar9 + -1;
        }
        if (pbVar10[-1] == 0x2e) {
          pbVar10 = pbVar10 + -1;
          iVar9 = iVar9 + -1;
        }
      }
      if ((*(int *)((int)param_1 + 0x2c) == 0) && ((*(ushort *)(param_1 + 7) & 8) != 0)) {
        *pbVar10 = 0x2e;
        pbVar10 = pbVar10 + 1;
        iVar9 = iVar9 + 1;
      }
      if ((cVar8 == 'G') || (cVar8 == 'E')) {
        bVar3 = 0x45;
      }
      else {
        bVar3 = 0x65;
      }
      *pbVar10 = bVar3;
      pbVar11 = pbVar10 + 1;
      if (local_158[0] - uVar4 == 2) {
        uVar4 = uVar5 + 2;
        *pbVar11 = local_158[uVar5 + 2];
        pbVar10[2] = 0x30;
        pbVar11 = pbVar10 + 3;
        iVar9 = iVar9 + 2;
      }
    }
    else {
      if (local_158[uVar5 + 1] == 0x2e) {
        bVar1 = true;
      }
      *pbVar10 = local_158[uVar5 + 1];
      pbVar11 = pbVar10 + 1;
    }
    iVar9 = iVar9 + 1;
    pbVar10 = pbVar11;
    uVar5 = uVar4;
  }
  if (((((*(ushort *)(param_1 + 7) & 8) == 0) && ((cVar8 == 'G' || (cVar8 == 'g')))) &&
      (local_2e[0] == '\x01')) && (bVar1)) {
    if (pbVar10[-1] == 0x2e) {
      pbVar10 = pbVar10 + -1;
      iVar9 = iVar9 + -1;
    }
    else if (pbVar10[-1] == 0x30) {
      for (; pbVar10[-1] == 0x30; pbVar10 = pbVar10 + -1) {
        iVar9 = iVar9 + -1;
      }
      if (pbVar10[-1] == 0x2e) {
        pbVar10 = pbVar10 + -1;
        iVar9 = iVar9 + -1;
      }
    }
  }
  if ((((*(int *)((int)param_1 + 0x2c) == 0) || (*(int *)((int)param_1 + 0x2c) == 1)) &&
      ((*(ushort *)(param_1 + 7) & 8) != 0)) && ((local_2e[0] == '\x01' && (!bVar1)))) {
    *pbVar10 = 0x2e;
    iVar9 = iVar9 + 1;
  }
  *(int *)((int)param_1 + 0x1c) = iVar9;
  *(undefined1 *)(*(int *)(param_1 + 2) + iVar9) = 0;
  if (((*(ushort *)(param_1 + 7) & 0x14) == 0x10) &&
     (*(int *)(param_1 + 3) =
           (*(int *)((int)param_1 + 0x34) - iVar9) - *(int *)((int)param_1 + 0x14),
     *(int *)(param_1 + 3) < 0)) {
    *(undefined4 *)(param_1 + 3) = 0;
  }
  return;
}




/* Função: FUN_10009e10 | Endereço: 10009e10 */

void FUN_10009e10(uint *param_1,char param_2)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  char *pcVar5;
  uint local_38;
  int local_34;
  uint local_30;
  undefined4 local_2c;
  char acStack_28 [24];
  
  if (param_2 == 'X') {
    pcVar5 = s_0123456789ABCDEF_1000d202;
  }
  else {
    pcVar5 = s_0123456789abcdef_1000d1f0;
  }
  if (param_2 == 'o') {
    uVar4 = 8;
  }
  else if ((param_2 == 'x') || (param_2 == 'X')) {
    uVar4 = 0x10;
  }
  else {
    uVar4 = 10;
  }
  uVar1 = *param_1;
  iVar2 = 0x18;
  if (((param_2 == 'd') || (param_2 == 'i')) && ((int)*param_1 < 0)) {
    uVar1 = -uVar1;
  }
  if (((uVar1 != 0) || (param_1[0xb] != 0)) || (uVar4 == 8)) {
    iVar2 = 0x17;
    acStack_28[0x17] = pcVar5[uVar1 - (uVar1 / uVar4) * uVar4];
  }
  *param_1 = uVar1 / uVar4;
  for (; (0 < (int)*param_1 && (0 < iVar2)); iVar2 = iVar2 + -1) {
    FUN_1000aed0(&local_38,*param_1,uVar4);
    local_30 = local_38;
    local_2c = local_34;
    *param_1 = local_38;
    acStack_28[iVar2 + -1] = pcVar5[local_34];
  }
  iVar3 = iVar2;
  if (((uVar4 == 8) && ((param_1[0xe] & 0x80000) != 0)) && (acStack_28[iVar2] != '0')) {
    iVar3 = iVar2 + -1;
    acStack_28[iVar2 + -1] = '0';
  }
  param_1[7] = 0x18 - iVar3;
  FUN_1000b3b4(param_1[4],acStack_28 + iVar3,param_1[7]);
  if ((int)param_1[7] < (int)param_1[0xb]) {
    param_1[6] = param_1[0xb] - param_1[7];
    *(ushort *)(param_1 + 0xe) = *(ushort *)(param_1 + 0xe) & 0xffef;
  }
  else if ((int)param_1[0xb] < 0) {
    if (((param_1[0xe] & 0x140000) == 0x100000) &&
       (uVar4 = ((param_1[0xd] - param_1[5]) - param_1[6]) - param_1[7], 0 < (int)uVar4)) {
      param_1[6] = uVar4;
    }
  }
  return;
}




/* Função: FUN_1000a030 | Endereço: 1000a030 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int FUN_1000a030(undefined4 param_1,int param_2,char *param_3,int *param_4)

{
  undefined **ppuVar1;
  int iVar2;
  int iVar3;
  char *pcVar4;
  int *piStack00000024;
  undefined **local_bc;
  undefined1 auStack_98 [60];
  char local_5c;
  undefined2 local_5b;
  undefined1 auStack_58 [8];
  undefined4 local_50;
  int local_4c;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  int local_2c;
  int local_28;
  int local_24;
  ushort local_20;
  char local_1e;
  
  ppuVar1 = &.TVect::InitGraf;
  local_5b = _DAT_1000c648;
  local_28 = 0;
  piStack00000024 = param_4;
  local_50 = param_1;
  local_4c = param_2;
  do {
    while( true ) {
      local_5c = '\0';
      iVar2 = FUN_1000b130(&local_5c,param_3,0x7fffffff,&local_5b);
      if (iVar2 < 1) {
        iVar2 = FUN_1000b424(param_3);
      }
      iVar3 = iVar2;
      if (local_5c == '%') {
        iVar3 = iVar2 + -1;
      }
      if (0 < iVar3) {
        local_4c = FUN_1000bdec(local_4c,param_3,iVar3);
        if (local_4c == 0) {
          return -1;
        }
        local_28 = local_28 + iVar3;
        ppuVar1 = local_bc;
      }
      param_3 = param_3 + iVar2;
      if (local_5c == '%') break;
      if (local_5c == '\0') {
        return local_28;
      }
    }
    local_30 = 0;
    local_34 = 0;
    local_38 = 0;
    local_3c = 0;
    local_40 = 0;
    local_44 = 0;
    local_20 = 0;
    while (iVar2 = thunk_FUN_1000b40c(s____0_1000d214,(int)*param_3), iVar2 != 0) {
      param_3 = param_3 + 1;
      local_20 = local_20 | (ushort)*(undefined4 *)(&DAT_1000d21c + (iVar2 + -0x1000d214) * 4);
    }
    if (*param_3 == '*') {
      local_24 = *piStack00000024;
      if (local_24 < 0) {
        local_24 = -local_24;
        local_20 = local_20 | 4;
      }
      param_3 = param_3 + 1;
      piStack00000024 = piStack00000024 + 1;
    }
    else {
      local_24 = 0;
      while ((PTR_DAT_1000cf68[*param_3] & 0x20) != 0) {
        if (local_24 < 0x7fffffff) {
          local_24 = (int)*param_3 + local_24 * 10 + -0x30;
        }
        param_3 = param_3 + 1;
      }
    }
    if (*param_3 == '.') {
      pcVar4 = param_3 + 1;
      if (*pcVar4 == '*') {
        pcVar4 = param_3 + 2;
        local_2c = *piStack00000024;
        piStack00000024 = piStack00000024 + 1;
      }
      else {
        local_2c = 0;
        while ((PTR_DAT_1000cf68[*pcVar4] & 0x20) != 0) {
          if (local_2c < 0x7fffffff) {
            local_2c = (int)*pcVar4 + local_2c * 10 + -0x30;
          }
          pcVar4 = pcVar4 + 1;
        }
      }
    }
    else {
      local_2c = -1;
      pcVar4 = param_3;
    }
    iVar2 = thunk_FUN_1000b40c(ppuVar1 + 0x1e1,(int)*pcVar4);
    if (iVar2 == 0) {
      local_1e = '\0';
    }
    else {
      local_1e = *pcVar4;
      pcVar4 = pcVar4 + 1;
    }
    param_3 = pcVar4 + 1;
    iVar2 = FUN_1000a348(auStack_58,&stack0x00000024,*pcVar4,auStack_98);
  } while ((iVar2 == 0) && (iVar2 = FUN_1000a9e4(auStack_58,auStack_98), -1 < iVar2));
  return -1;
}




/* Função: FUN_1000a348 | Endereço: 1000a348 */

undefined4 FUN_1000a348(uint *param_1,int *param_2,undefined4 param_3,int param_4)

{
  uint *puVar1;
  undefined8 *puVar2;
  int iVar3;
  uint uVar4;
  undefined4 uVar5;
  char cVar7;
  undefined4 *puVar6;
  undefined8 uVar8;
  undefined1 uStack_18;
  undefined1 uStack_17;
  
  cVar7 = (char)param_3;
  switch(cVar7) {
  case '%':
    uVar4 = param_1[5];
    param_1[5] = uVar4 + 1;
    *(undefined1 *)(param_4 + uVar4) = 0x25;
    break;
  default:
    if (cVar7 == '\0') {
      cVar7 = '%';
    }
    uVar4 = param_1[5];
    param_1[5] = uVar4 + 1;
    *(char *)(param_4 + uVar4) = cVar7;
    break;
  case 'E':
  case 'G':
  case 'e':
  case 'f':
  case 'g':
    if (*(char *)((int)param_1 + 0x3a) == 'L') {
      puVar2 = (undefined8 *)*param_2;
      *param_2 = (int)(puVar2 + 1);
      uVar8 = *puVar2;
    }
    else {
      puVar2 = (undefined8 *)*param_2;
      *param_2 = (int)(puVar2 + 1);
      uVar8 = *puVar2;
    }
    *(undefined8 *)param_1 = uVar8;
    if ((*param_1 & 0x80000000) == 0) {
      if ((param_1[0xe] & 0x20000) == 0) {
        if ((param_1[0xe] & 0x10000) != 0) {
          uVar4 = param_1[5];
          param_1[5] = uVar4 + 1;
          *(undefined1 *)(param_4 + uVar4) = 0x20;
        }
      }
      else {
        uVar4 = param_1[5];
        param_1[5] = uVar4 + 1;
        *(undefined1 *)(param_4 + uVar4) = 0x2b;
      }
    }
    else {
      uVar4 = param_1[5];
      param_1[5] = uVar4 + 1;
      *(undefined1 *)(param_4 + uVar4) = 0x2d;
    }
    param_1[4] = param_4 + param_1[5];
    FUN_1000998c(param_1,param_3);
    break;
  case 'X':
  case 'o':
  case 'u':
  case 'x':
    if (*(char *)((int)param_1 + 0x3a) == 'l') {
      puVar1 = (uint *)*param_2;
      *param_2 = (int)(puVar1 + 1);
      uVar4 = *puVar1;
    }
    else {
      puVar1 = (uint *)*param_2;
      *param_2 = (int)(puVar1 + 1);
      uVar4 = *puVar1;
    }
    *param_1 = uVar4;
    if (*(char *)((int)param_1 + 0x3a) == 'h') {
      *param_1 = *param_1 & 0xffff;
    }
    else if (*(char *)((int)param_1 + 0x3a) == '\0') {
      *param_1 = *param_1;
    }
    if ((((param_1[0xe] & 0x80000) != 0) && (*param_1 != 0)) && ((cVar7 == 'x' || (cVar7 == 'X'))))
    {
      uVar4 = param_1[5];
      param_1[5] = uVar4 + 1;
      *(undefined1 *)(param_4 + uVar4) = 0x30;
      uVar4 = param_1[5];
      param_1[5] = uVar4 + 1;
      *(char *)(param_4 + uVar4) = cVar7;
    }
    param_1[4] = param_4 + param_1[5];
    FUN_10009e10(param_1,param_3);
    break;
  case 'c':
    if (*(char *)((int)param_1 + 0x3a) == 'l') {
      puVar6 = (undefined4 *)*param_2;
      *param_2 = (int)(puVar6 + 1);
      uStack_18 = (undefined1)*puVar6;
      uStack_17 = 0;
      param_1[0xb] = 0xffffffff;
      iVar3 = FUN_1000a848(param_1,&uStack_18);
      if (iVar3 < 0) {
        return 0xffffffff;
      }
    }
    else {
      puVar6 = (undefined4 *)*param_2;
      *param_2 = (int)(puVar6 + 1);
      uVar4 = param_1[5];
      uVar5 = *puVar6;
      param_1[5] = uVar4 + 1;
      *(char *)(param_4 + uVar4) = (char)uVar5;
    }
    break;
  case 'd':
  case 'i':
    if (*(char *)((int)param_1 + 0x3a) == 'l') {
      puVar1 = (uint *)*param_2;
      *param_2 = (int)(puVar1 + 1);
      uVar4 = *puVar1;
    }
    else {
      puVar1 = (uint *)*param_2;
      *param_2 = (int)(puVar1 + 1);
      uVar4 = *puVar1;
    }
    *param_1 = uVar4;
    if (*(char *)((int)param_1 + 0x3a) == 'h') {
      *param_1 = (int)(short)*param_1;
    }
    if ((int)*param_1 < 0) {
      uVar4 = param_1[5];
      param_1[5] = uVar4 + 1;
      *(undefined1 *)(param_4 + uVar4) = 0x2d;
    }
    else if ((param_1[0xe] & 0x20000) == 0) {
      if ((param_1[0xe] & 0x10000) != 0) {
        uVar4 = param_1[5];
        param_1[5] = uVar4 + 1;
        *(undefined1 *)(param_4 + uVar4) = 0x20;
      }
    }
    else {
      uVar4 = param_1[5];
      param_1[5] = uVar4 + 1;
      *(undefined1 *)(param_4 + uVar4) = 0x2b;
    }
    param_1[4] = param_4 + param_1[5];
    FUN_10009e10(param_1,param_3);
    break;
  case 'n':
    if (*(char *)((int)param_1 + 0x3a) == 'h') {
      puVar6 = (undefined4 *)*param_2;
      uVar4 = param_1[0xc];
      *param_2 = (int)(puVar6 + 1);
      *(short *)*puVar6 = (short)uVar4;
    }
    else if (*(char *)((int)param_1 + 0x3a) == 'l') {
      puVar6 = (undefined4 *)*param_2;
      uVar4 = param_1[0xc];
      *param_2 = (int)(puVar6 + 1);
      *(uint *)*puVar6 = uVar4;
    }
    else {
      puVar6 = (undefined4 *)*param_2;
      uVar4 = param_1[0xc];
      *param_2 = (int)(puVar6 + 1);
      *(uint *)*puVar6 = uVar4;
    }
    break;
  case 'p':
    puVar1 = (uint *)*param_2;
    *param_2 = (int)(puVar1 + 1);
    *param_1 = *puVar1;
    param_1[4] = param_4 + param_1[5];
    FUN_10009e10(param_1,0x78);
    break;
  case 's':
    if (*(char *)((int)param_1 + 0x3a) == 'l') {
      puVar6 = (undefined4 *)*param_2;
      *param_2 = (int)(puVar6 + 1);
      iVar3 = FUN_1000a848(param_1,*puVar6);
      if (iVar3 < 0) {
        return 0xffffffff;
      }
    }
    else {
      puVar1 = (uint *)*param_2;
      *param_2 = (int)(puVar1 + 1);
      param_1[4] = *puVar1;
      if ((param_1[0xe] & 0x80000) == 0) {
        if ((int)param_1[0xb] < 0) {
          uVar4 = FUN_1000b424(param_1[4]);
        }
        else {
          iVar3 = thunk_FUN_1000b3a4(param_1[4],0,param_1[0xb]);
          if (iVar3 == 0) {
            uVar4 = param_1[0xb];
          }
          else {
            uVar4 = iVar3 - param_1[4];
          }
        }
        param_1[7] = uVar4;
      }
      else {
        if ((int)param_1[0xb] < 0) {
          uVar4 = (uint)*(byte *)param_1[4];
        }
        else if ((int)(uint)*(byte *)param_1[4] < (int)param_1[0xb]) {
          uVar4 = (uint)*(byte *)param_1[4];
        }
        else {
          uVar4 = param_1[0xb];
        }
        param_1[7] = uVar4;
        param_1[4] = param_1[4] + 1;
      }
    }
  }
  return 0;
}




/* Função: FUN_1000a848 | Endereço: 1000a848 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_1000a848(int param_1,char *param_2)

{
  undefined1 *puVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
  int iVar5;
  undefined2 local_78;
  undefined1 auStack_76 [8];
  undefined1 auStack_6e [82];
  
  if (*(int *)(param_1 + 0x2c) < 0) {
    iVar5 = 0x7fffffff;
  }
  else {
    iVar5 = *(int *)(param_1 + 0x2c);
  }
  local_78 = _DAT_1000c64a;
  uVar3 = *(int *)(param_1 + 0x34) + (int)DAT_1000d16e;
  if (((*(ushort *)(param_1 + 0x38) & 4) == 0) && (0x40 < uVar3)) {
    puVar1 = (undefined1 *)FUN_1000b030(uVar3);
    if (puVar1 == (undefined1 *)0x0) {
      return 0xffffffff;
    }
  }
  else {
    puVar1 = auStack_6e;
    uVar3 = 0x40;
  }
  uVar4 = 0;
  do {
    if (iVar5 < 1) {
LAB_1000a998:
      FUN_1000a9e4(param_1,puVar1);
      *(undefined4 *)(param_1 + 0x34) = 0;
      *(undefined4 *)(param_1 + 0x14) = 0;
      if (puVar1 != auStack_6e) {
        FUN_1000adb4(puVar1);
      }
      return uVar4;
    }
    iVar2 = FUN_1000b254(auStack_76,*param_2,&local_78);
    if (iVar2 < 1) {
      uVar4 = 0xffffffff;
      goto LAB_1000a998;
    }
    if (*param_2 == '\0') {
      iVar2 = iVar2 + -1;
    }
    if (iVar5 < iVar2) goto LAB_1000a998;
    if (uVar3 < (uint)(*(int *)(param_1 + 0x14) + iVar2)) {
      *(undefined4 *)(param_1 + 0x34) = 0;
      FUN_1000a9e4(param_1,puVar1);
      *(undefined4 *)(param_1 + 0x14) = 0;
    }
    FUN_1000b3b4(puVar1 + *(int *)(param_1 + 0x14),auStack_76,iVar2);
    *(int *)(param_1 + 0x14) = *(int *)(param_1 + 0x14) + iVar2;
    if (*param_2 == '\0') goto LAB_1000a998;
    param_2 = param_2 + 1;
    iVar5 = iVar5 - iVar2;
  } while( true );
}




/* Função: FUN_1000a9e4 | Endereço: 1000a9e4 */

undefined4 FUN_1000a9e4(int param_1,undefined4 param_2)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  
  uVar4 = (((((*(int *)(param_1 + 0x34) - *(int *)(param_1 + 0x14)) - *(int *)(param_1 + 0x18)) -
            *(int *)(param_1 + 0x1c)) - *(int *)(param_1 + 0x20)) - *(int *)(param_1 + 0x24)) -
          *(int *)(param_1 + 0x28);
  if (((*(ushort *)(param_1 + 0x38) & 4) == 0) && (uVar3 = uVar4, 0 < (int)uVar4)) {
    for (; 0 < (int)uVar3; uVar3 = uVar3 - uVar2) {
      uVar2 = uVar3;
      if (0x20 < uVar3) {
        uVar2 = 0x20;
      }
      if (0 < (int)uVar2) {
        iVar1 = FUN_1000bdec(*(undefined4 *)(param_1 + 0xc),s__1000d384,uVar2);
        *(int *)(param_1 + 0xc) = iVar1;
        if (iVar1 == 0) {
          return 0xffffffff;
        }
        *(uint *)(param_1 + 0x30) = *(int *)(param_1 + 0x30) + uVar2;
      }
    }
  }
  if (0 < *(int *)(param_1 + 0x14)) {
    iVar1 = FUN_1000bdec(*(undefined4 *)(param_1 + 0xc),param_2,*(undefined4 *)(param_1 + 0x14));
    *(int *)(param_1 + 0xc) = iVar1;
    if (iVar1 == 0) {
      return 0xffffffff;
    }
    *(int *)(param_1 + 0x30) = *(int *)(param_1 + 0x30) + *(int *)(param_1 + 0x14);
  }
  if (0 < *(int *)(param_1 + 0x18)) {
    for (uVar3 = *(uint *)(param_1 + 0x18); 0 < (int)uVar3; uVar3 = uVar3 - uVar2) {
      uVar2 = uVar3;
      if (0x20 < uVar3) {
        uVar2 = 0x20;
      }
      if (0 < (int)uVar2) {
        iVar1 = FUN_1000bdec(*(undefined4 *)(param_1 + 0xc),
                             s_00000000000000000000000000000000_1000d3a6,uVar2);
        *(int *)(param_1 + 0xc) = iVar1;
        if (iVar1 == 0) {
          return 0xffffffff;
        }
        *(uint *)(param_1 + 0x30) = *(int *)(param_1 + 0x30) + uVar2;
      }
    }
  }
  if (0 < *(int *)(param_1 + 0x1c)) {
    iVar1 = FUN_1000bdec(*(undefined4 *)(param_1 + 0xc),*(undefined4 *)(param_1 + 0x10),
                         *(undefined4 *)(param_1 + 0x1c));
    *(int *)(param_1 + 0xc) = iVar1;
    if (iVar1 == 0) {
      return 0xffffffff;
    }
    *(int *)(param_1 + 0x30) = *(int *)(param_1 + 0x30) + *(int *)(param_1 + 0x1c);
  }
  if (0 < *(int *)(param_1 + 0x20)) {
    for (uVar3 = *(uint *)(param_1 + 0x20); 0 < (int)uVar3; uVar3 = uVar3 - uVar2) {
      uVar2 = uVar3;
      if (0x20 < uVar3) {
        uVar2 = 0x20;
      }
      if (0 < (int)uVar2) {
        iVar1 = FUN_1000bdec(*(undefined4 *)(param_1 + 0xc),
                             s_00000000000000000000000000000000_1000d3a6,uVar2);
        *(int *)(param_1 + 0xc) = iVar1;
        if (iVar1 == 0) {
          return 0xffffffff;
        }
        *(uint *)(param_1 + 0x30) = *(int *)(param_1 + 0x30) + uVar2;
      }
    }
  }
  if (0 < *(int *)(param_1 + 0x24)) {
    iVar1 = FUN_1000bdec(*(undefined4 *)(param_1 + 0xc),
                         *(int *)(param_1 + 0x10) + *(int *)(param_1 + 0x1c),
                         *(undefined4 *)(param_1 + 0x24));
    *(int *)(param_1 + 0xc) = iVar1;
    if (iVar1 == 0) {
      return 0xffffffff;
    }
    *(int *)(param_1 + 0x30) = *(int *)(param_1 + 0x30) + *(int *)(param_1 + 0x24);
  }
  if (0 < *(int *)(param_1 + 0x28)) {
    for (uVar3 = *(uint *)(param_1 + 0x28); 0 < (int)uVar3; uVar3 = uVar3 - uVar2) {
      uVar2 = uVar3;
      if (0x20 < uVar3) {
        uVar2 = 0x20;
      }
      if (0 < (int)uVar2) {
        iVar1 = FUN_1000bdec(*(undefined4 *)(param_1 + 0xc),
                             s_00000000000000000000000000000000_1000d3a6,uVar2);
        *(int *)(param_1 + 0xc) = iVar1;
        if (iVar1 == 0) {
          return 0xffffffff;
        }
        *(uint *)(param_1 + 0x30) = *(int *)(param_1 + 0x30) + uVar2;
      }
    }
  }
  if (((*(ushort *)(param_1 + 0x38) & 4) != 0) && (0 < (int)uVar4)) {
    for (; 0 < (int)uVar4; uVar4 = uVar4 - uVar3) {
      uVar3 = uVar4;
      if (0x20 < uVar4) {
        uVar3 = 0x20;
      }
      if (0 < (int)uVar3) {
        iVar1 = FUN_1000bdec(*(undefined4 *)(param_1 + 0xc),s__1000d384,uVar3);
        *(int *)(param_1 + 0xc) = iVar1;
        if (iVar1 == 0) {
          return 0xffffffff;
        }
        *(uint *)(param_1 + 0x30) = *(int *)(param_1 + 0x30) + uVar3;
      }
    }
  }
  return 0;
}




/* Função: FUN_1000ada4 | Endereço: 1000ada4 */

int FUN_1000ada4(int param_1)

{
  if (-1 < param_1) {
    return param_1;
  }
  return -param_1;
}




/* Função: FUN_1000adb4 | Endereço: 1000adb4 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_1000adb4(int param_1)

{
  uint *puVar1;
  uint *puVar2;
  
  if (param_1 == 0) {
    return;
  }
  puVar1 = (uint *)(param_1 - 8);
  if (*puVar1 < 8) {
    return;
  }
  if ((*puVar1 & 7) != 0) {
    return;
  }
  if ((puRam1000dc40 == (uint *)0x0) || (puVar2 = puRam1000dc40, puVar1 < puRam1000dc40)) {
    *(uint **)(param_1 + -4) = puRam1000dc40;
    puRam1000dc40 = puVar1;
  }
  else {
    for (; (puVar2[1] != 0 && ((uint *)puVar2[1] < puVar1)); puVar2 = (uint *)puVar2[1]) {
    }
    if (puVar1 < (uint *)((int)puVar2 + *puVar2)) {
      return;
    }
    if ((uint *)((int)puVar2 + *puVar2) == puVar1) {
      *puVar2 = *puVar2 + *puVar1;
      puVar1 = puVar2;
    }
    else {
      if ((puVar2[1] != 0) && (puVar2[1] < (int)puVar1 + *puVar1)) {
        return;
      }
      *(uint *)(param_1 + -4) = puVar2[1];
      puVar2[1] = (uint)puVar1;
    }
  }
  if (puVar1[1] == 0) {
    return;
  }
  if ((int)puVar1 + *puVar1 == puVar1[1]) {
    _DAT_1000dc3c = 0;
    *puVar1 = *puVar1 + *(int *)puVar1[1];
    puVar1[1] = *(uint *)(puVar1[1] + 4);
    return;
  }
  return;
}




/* Função: FUN_1000aed0 | Endereço: 1000aed0 */

void FUN_1000aed0(int *param_1,int param_2,int param_3)

{
  *param_1 = param_2 / param_3;
  param_1[1] = param_2 - param_3 * (param_2 / param_3);
  return;
}




/* Função: FUN_1000aefc | Endereço: 1000aefc */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int * FUN_1000aefc(uint param_1)

{
  int *piVar1;
  undefined *puVar2;
  uint *puVar3;
  uint uVar4;
  
  do {
    piVar1 = _DAT_1000dc3c;
    puVar2 = &DAT_1000dc3c;
    if (_DAT_1000dc3c == (int *)0x0) {
      while (piVar1 = (int *)(puVar2 + 4), *piVar1 != 0) {
        if (param_1 <= *(uint *)*piVar1) {
          return piVar1;
        }
        puVar2 = (undefined *)*piVar1;
      }
    }
    else {
      for (; *piVar1 != 0; piVar1 = (int *)(*piVar1 + 4)) {
        if (param_1 <= *(uint *)*piVar1) {
          return piVar1;
        }
      }
      while (piVar1 = (int *)(puVar2 + 4), *piVar1 != *_DAT_1000dc3c) {
        if (param_1 <= *(uint *)*piVar1) {
          return piVar1;
        }
        puVar2 = (undefined *)*piVar1;
      }
    }
    uVar4 = 0x8000;
    while( true ) {
      if (uVar4 < param_1) {
        uVar4 = param_1;
      }
      puVar3 = (uint *)FUN_1000b0f8(uVar4);
      if (puVar3 != (uint *)0x0) break;
      if (uVar4 == param_1) {
        return (int *)0x0;
      }
      uVar4 = uVar4 >> 1;
    }
    *puVar3 = uVar4;
    FUN_1000adb4(puVar3 + 2);
  } while( true );
}




/* Função: FUN_1000b030 | Endereço: 1000b030 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint * FUN_1000b030(uint param_1)

{
  uint *puVar1;
  uint *puVar2;
  uint uVar3;
  
  uVar3 = param_1 + 0xf & 0xfffffff8;
  if (param_1 == 0) {
    puVar1 = (uint *)0x0;
  }
  else if (param_1 < uVar3) {
    if (uVar3 < 8) {
      uVar3 = 8;
    }
    puVar2 = (uint *)FUN_1000aefc(uVar3);
    if (puVar2 == (uint *)0x0) {
      puVar1 = (uint *)0x0;
    }
    else {
      puVar1 = (uint *)*puVar2;
      if (*puVar1 - 8 < uVar3) {
        *puVar2 = puVar1[1];
      }
      else {
        *puVar2 = (int)puVar1 + uVar3;
        *(uint *)(*puVar2 + 4) = puVar1[1];
        *(uint *)*puVar2 = *puVar1 - uVar3;
        *puVar1 = uVar3;
      }
      puVar1 = puVar1 + 2;
      _DAT_1000dc3c = puVar2;
    }
  }
  else {
    puVar1 = (uint *)0x0;
  }
  return puVar1;
}




/* Função: FUN_1000b0f8 | Endereço: 1000b0f8 */

int FUN_1000b0f8(int param_1)

{
  int iVar1;
  
  if ((param_1 == 0) || (iVar1 = .glue::NewPtr(), iVar1 == 0)) {
    iVar1 = 0;
  }
  return iVar1;
}




/* Função: FUN_1000b130 | Endereço: 1000b130 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint FUN_1000b130(byte *param_1,byte *param_2,int param_3,byte *param_4)

{
  ushort uVar1;
  byte bVar2;
  uint uVar3;
  uint uVar4;
  byte *pbVar5;
  ushort uVar6;
  
  if (param_2 == (byte *)0x0) {
    *(undefined2 *)param_4 = _DAT_1000c64c;
    uVar3 = *(ushort *)PTR_DAT_1000d170 & 0xf00;
  }
  else {
    uVar3 = (uint)param_4[1];
    uVar6 = (ushort)*param_4;
    uVar4 = 0;
    pbVar5 = param_2;
    do {
      bVar2 = (byte)uVar3;
      if (param_3 == 0) {
        *param_4 = (byte)uVar6;
        param_4[1] = bVar2;
        return 0xfffffffe;
      }
      if (((('\x0f' < (char)bVar2) || ((&PTR_DAT_1000d170)[(char)bVar2] == (undefined *)0x0)) ||
          (uVar4 = uVar4 + 1, 0xfef < uVar4)) ||
         (uVar1 = *(ushort *)((&PTR_DAT_1000d170)[(char)bVar2] + (uint)*pbVar5 * 2), uVar1 == 0)) {
        _DAT_1000dc38 = 0x24;
        return 0xffffffff;
      }
      uVar3 = (int)(uVar1 & 0xf00) >> 8;
      if ((uVar1 & 0x8000) != 0) {
        uVar6 = uVar1 & 0xff;
      }
      if (((uVar1 & 0x4000) != 0) && (*pbVar5 != 0)) {
        pbVar5 = pbVar5 + 1;
        param_3 = param_3 + -1;
        uVar4 = 0;
      }
    } while ((uVar1 & 0x2000) == 0);
    if (param_1 != (byte *)0x0) {
      *param_1 = (byte)uVar6;
    }
    *param_4 = (byte)uVar6;
    param_4[1] = (byte)((uVar1 & 0xf00) >> 8);
    uVar3 = (int)pbVar5 - (int)param_2;
  }
  return uVar3;
}




/* Função: FUN_1000b254 | Endereço: 1000b254 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint FUN_1000b254(int param_1,uint param_2,undefined2 *param_3)

{
  ushort uVar1;
  bool bVar2;
  uint uVar3;
  uint uVar4;
  char cVar5;
  uint uVar6;
  
  if (param_1 == 0) {
    *param_3 = _DAT_1000c64e;
    uVar4 = *(ushort *)PTR_DAT_1000d170 & 0xf00;
  }
  else {
    uVar3 = (uint)*(byte *)((int)param_3 + 1);
    bVar2 = false;
    uVar6 = 0;
    uVar4 = 0;
    do {
      if (((('\x0f' < (char)uVar3) || ((&PTR_DAT_1000d1b0)[(char)uVar3] == (undefined *)0x0)) ||
          ((int)DAT_1000d16e <= (int)uVar4)) ||
         ((uVar6 = uVar6 + 1, 0xfef < uVar6 ||
          (uVar1 = *(ushort *)((&PTR_DAT_1000d1b0)[(char)uVar3] + (param_2 & 0xff) * 2), uVar1 == 0)
          ))) {
        _DAT_1000dc38 = 0x24;
        return 0xffffffff;
      }
      uVar3 = (int)(uVar1 & 0xf00) >> 8;
      if ((uVar1 & 0x8000) != 0) {
        param_2 = uVar1 & 0xff;
      }
      if ((uVar1 & 0x1000) != 0) {
        param_2 = param_2 & 0xff;
      }
      if ((uVar1 & 0x2000) != 0) {
        if ((uVar1 & 0xff) == 0) {
          cVar5 = (char)param_2;
        }
        else {
          cVar5 = (char)uVar1;
        }
        *(char *)(param_1 + uVar4) = cVar5;
        uVar4 = uVar4 + 1;
        if (cVar5 == '\0') {
          bVar2 = true;
        }
        uVar6 = 0;
      }
    } while (((uVar1 & 0x4000) == 0) && (!bVar2));
    *(char *)((int)param_3 + 1) = (char)((uVar1 & 0xf00) >> 8);
  }
  return uVar4;
}




/* Função: thunk_FUN_1000b3a4 | Endereço: 1000b384 */

char * thunk_FUN_1000b3a4(char *param_1,char param_2,int param_3)

{
  while( true ) {
    if (param_3 == 0) {
      return (char *)0x0;
    }
    if (*param_1 == param_2) break;
    param_1 = param_1 + 1;
    param_3 = param_3 + -1;
  }
  return param_1;
}




/* Função: FUN_1000b3a4 | Endereço: 1000b3a4 */

char * FUN_1000b3a4(char *param_1,char param_2,int param_3)

{
  while( true ) {
    if (param_3 == 0) {
      return (char *)0x0;
    }
    if (*param_1 == param_2) break;
    param_1 = param_1 + 1;
    param_3 = param_3 + -1;
  }
  return param_1;
}




/* Função: FUN_1000b3b4 | Endereço: 1000b3b4 */

undefined4 FUN_1000b3b4(undefined4 param_1,undefined4 param_2)

{
  .glue::BlockMoveData(param_2,param_1);
  return param_1;
}




/* Função: thunk_FUN_1000b40c | Endereço: 1000b3f0 */

char * thunk_FUN_1000b40c(char *param_1,char param_2)

{
  while( true ) {
    if (*param_1 == param_2) {
      return param_1;
    }
    if (*param_1 == '\0') break;
    param_1 = param_1 + 1;
  }
  return (char *)0x0;
}




/* Função: FUN_1000b40c | Endereço: 1000b40c */

char * FUN_1000b40c(char *param_1,char param_2)

{
  while( true ) {
    if (*param_1 == param_2) {
      return param_1;
    }
    if (*param_1 == '\0') break;
    param_1 = param_1 + 1;
  }
  return (char *)0x0;
}




/* Função: FUN_1000b424 | Endereço: 1000b424 */

int FUN_1000b424(char *param_1)

{
  char *pcVar1;
  
  pcVar1 = param_1;
  while( true ) {
    if (*pcVar1 == '\0') break;
    pcVar1 = pcVar1 + 1;
  }
  return (int)pcVar1 - (int)param_1;
}




/* Função: MaxApplZone | Endereço: 1000b444 */

void .glue::MaxApplZone(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b458. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::MaxApplZone)();
  return;
}




/* Função: MoreMasters | Endereço: 1000b45c */

void .glue::MoreMasters(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b470. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::MoreMasters)();
  return;
}




/* Função: InitGraf | Endereço: 1000b474 */

void .glue::InitGraf(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b488. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::InitGraf)();
  return;
}




/* Função: InitFonts | Endereço: 1000b48c */

void .glue::InitFonts(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b4a0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::InitFonts)();
  return;
}




/* Função: InitWindows | Endereço: 1000b4a4 */

void .glue::InitWindows(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b4b8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::InitWindows)();
  return;
}




/* Função: InitMenus | Endereço: 1000b4bc */

void .glue::InitMenus(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b4d0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::InitMenus)();
  return;
}




/* Função: TEInit | Endereço: 1000b4d4 */

void .glue::TEInit(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b4e8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::TEInit)();
  return;
}




/* Função: InitDialogs | Endereço: 1000b4ec */

void .glue::InitDialogs(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b500. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::InitDialogs)();
  return;
}




/* Função: FlushEvents | Endereço: 1000b504 */

void .glue::FlushEvents(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b518. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::FlushEvents)();
  return;
}




/* Função: InitCursor | Endereço: 1000b51c */

void .glue::InitCursor(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b530. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::InitCursor)();
  return;
}




/* Função: GetResource | Endereço: 1000b534 */

void .glue::GetResource(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b548. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::GetResource)();
  return;
}




/* Função: ResError | Endereço: 1000b54c */

void .glue::ResError(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b560. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::ResError)();
  return;
}




/* Função: HomeResFile | Endereço: 1000b564 */

void .glue::HomeResFile(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b578. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::HomeResFile)();
  return;
}




/* Função: ReleaseResource | Endereço: 1000b57c */

void .glue::ReleaseResource(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b590. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::ReleaseResource)();
  return;
}




/* Função: UseResFile | Endereço: 1000b594 */

void .glue::UseResFile(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b5a8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::UseResFile)();
  return;
}




/* Função: GetPicture | Endereço: 1000b5ac */

void .glue::GetPicture(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b5c0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::GetPicture)();
  return;
}




/* Função: GetNewMBar | Endereço: 1000b5c4 */

void .glue::GetNewMBar(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b5d8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::GetNewMBar)();
  return;
}




/* Função: SetMenuBar | Endereço: 1000b5dc */

void .glue::SetMenuBar(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b5f0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::SetMenuBar)();
  return;
}




/* Função: GetMenuHandle | Endereço: 1000b5f4 */

void .glue::GetMenuHandle(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b608. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::GetMenuHandle)();
  return;
}




/* Função: AppendResMenu | Endereço: 1000b60c */

void .glue::AppendResMenu(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b620. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::AppendResMenu)();
  return;
}




/* Função: DrawMenuBar | Endereço: 1000b624 */

void .glue::DrawMenuBar(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b638. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::DrawMenuBar)();
  return;
}




/* Função: NGetTrapAddress | Endereço: 1000b63c */

void .glue::NGetTrapAddress(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b650. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::NGetTrapAddress)();
  return;
}




/* Função: Alert | Endereço: 1000b654 */

void .glue::Alert(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b668. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::Alert)();
  return;
}




/* Função: GetMouse | Endereço: 1000b66c */

void .glue::GetMouse(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b680. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::GetMouse)();
  return;
}




/* Função: GetDialogItem | Endereço: 1000b684 */

void .glue::GetDialogItem(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b698. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::GetDialogItem)();
  return;
}




/* Função: PtInRect | Endereço: 1000b69c */

void .glue::PtInRect(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b6b0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::PtInRect)();
  return;
}




/* Função: TrackControl | Endereço: 1000b6b4 */

void .glue::TrackControl(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b6c8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::TrackControl)();
  return;
}




/* Função: NewRoutineDescriptor | Endereço: 1000b6cc */

void .glue::NewRoutineDescriptor(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b6e0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::NewRoutineDescriptor)();
  return;
}




/* Função: GetPort | Endereço: 1000b6e4 */

void .glue::GetPort(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b6f8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::GetPort)();
  return;
}




/* Função: GetNewDialog | Endereço: 1000b6fc */

void .glue::GetNewDialog(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b710. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::GetNewDialog)();
  return;
}




/* Função: SetWTitle | Endereço: 1000b714 */

void .glue::SetWTitle(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b728. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::SetWTitle)();
  return;
}




/* Função: SetPort | Endereço: 1000b72c */

void .glue::SetPort(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b740. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::SetPort)();
  return;
}




/* Função: TextFont | Endereço: 1000b744 */

void .glue::TextFont(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b758. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::TextFont)();
  return;
}




/* Função: TextSize | Endereço: 1000b75c */

void .glue::TextSize(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b770. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::TextSize)();
  return;
}




/* Função: ShowWindow | Endereço: 1000b774 */

void .glue::ShowWindow(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b788. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::ShowWindow)();
  return;
}




/* Função: DrawControls | Endereço: 1000b78c */

void .glue::DrawControls(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b7a0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::DrawControls)();
  return;
}




/* Função: SystemTask | Endereço: 1000b7a4 */

void .glue::SystemTask(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b7b8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::SystemTask)();
  return;
}




/* Função: WaitNextEvent | Endereço: 1000b7bc */

void .glue::WaitNextEvent(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b7d0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::WaitNextEvent)();
  return;
}




/* Função: IsDialogEvent | Endereço: 1000b7d4 */

void .glue::IsDialogEvent(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b7e8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::IsDialogEvent)();
  return;
}




/* Função: FrontWindow | Endereço: 1000b7ec */

void .glue::FrontWindow(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b800. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::FrontWindow)();
  return;
}




/* Função: CallUniversalProc | Endereço: 1000b804 */

void .glue::CallUniversalProc(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b818. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::CallUniversalProc)();
  return;
}




/* Função: DialogSelect | Endereço: 1000b81c */

void .glue::DialogSelect(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b830. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::DialogSelect)();
  return;
}




/* Função: FindWindow | Endereço: 1000b834 */

void .glue::FindWindow(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b848. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::FindWindow)();
  return;
}




/* Função: MenuSelect | Endereço: 1000b84c */

void .glue::MenuSelect(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b860. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::MenuSelect)();
  return;
}




/* Função: ModalDialog | Endereço: 1000b864 */

void .glue::ModalDialog(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b878. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::ModalDialog)();
  return;
}




/* Função: DisposeDialog | Endereço: 1000b87c */

void .glue::DisposeDialog(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b890. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::DisposeDialog)();
  return;
}




/* Função: GetMenuItemText | Endereço: 1000b894 */

void .glue::GetMenuItemText(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b8a8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::GetMenuItemText)();
  return;
}




/* Função: OpenDeskAcc | Endereço: 1000b8ac */

void .glue::OpenDeskAcc(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b8c0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::OpenDeskAcc)();
  return;
}




/* Função: HiliteMenu | Endereço: 1000b8c4 */

void .glue::HiliteMenu(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b8d8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::HiliteMenu)();
  return;
}




/* Função: DragWindow | Endereço: 1000b8dc */

void .glue::DragWindow(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b8f0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::DragWindow)();
  return;
}




/* Função: DrawPicture | Endereço: 1000b8f4 */

void .glue::DrawPicture(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b908. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::DrawPicture)();
  return;
}




/* Função: PinRect | Endereço: 1000b90c */

void .glue::PinRect(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b920. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::PinRect)();
  return;
}




/* Função: Button | Endereço: 1000b924 */

void .glue::Button(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b938. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::Button)();
  return;
}




/* Função: GetControlValue | Endereço: 1000b93c */

void .glue::GetControlValue(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b950. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::GetControlValue)();
  return;
}




/* Função: MoveTo | Endereço: 1000b954 */

void .glue::MoveTo(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b968. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::MoveTo)();
  return;
}




/* Função: DrawString | Endereço: 1000b96c */

void .glue::DrawString(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b980. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::DrawString)();
  return;
}




/* Função: SetPt | Endereço: 1000b984 */

void .glue::SetPt(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b998. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::SetPt)();
  return;
}




/* Função: SFGetFile | Endereço: 1000b99c */

void .glue::SFGetFile(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b9b0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::SFGetFile)();
  return;
}




/* Função: BlockMove | Endereço: 1000b9b4 */

void .glue::BlockMove(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b9c8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::BlockMove)();
  return;
}




/* Função: DisposeRoutineDescriptor | Endereço: 1000b9cc */

void .glue::DisposeRoutineDescriptor(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b9e0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::DisposeRoutineDescriptor)();
  return;
}




/* Função: FSOpen | Endereço: 1000b9e4 */

void .glue::FSOpen(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000b9f8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::FSOpen)();
  return;
}




/* Função: GetFPos | Endereço: 1000b9fc */

void .glue::GetFPos(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000ba10. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::GetFPos)();
  return;
}




/* Função: SetFPos | Endereço: 1000ba14 */

void .glue::SetFPos(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000ba28. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::SetFPos)();
  return;
}




/* Função: FSRead | Endereço: 1000ba2c */

void .glue::FSRead(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000ba40. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::FSRead)();
  return;
}




/* Função: pow | Endereço: 1000ba44 */

double .glue::pow(double __x,double __y)

{
  double dVar1;
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000ba58. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  dVar1 = (double)(**(code **).TVect::pow)();
  return dVar1;
}




/* Função: DrawDialog | Endereço: 1000ba5c */

void .glue::DrawDialog(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000ba70. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::DrawDialog)();
  return;
}




/* Função: FSClose | Endereço: 1000ba74 */

void .glue::FSClose(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000ba88. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::FSClose)();
  return;
}




/* Função: BlockMoveData | Endereço: 1000ba8c */

void .glue::BlockMoveData(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000baa0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::BlockMoveData)();
  return;
}




/* Função: NewPtr | Endereço: 1000baa4 */

void .glue::NewPtr(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bab8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::NewPtr)();
  return;
}




/* Função: num2dec | Endereço: 1000babc */

void .glue::num2dec(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bad0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::num2dec)();
  return;
}




/* Função: dec2str | Endereço: 1000bad4 */

void .glue::dec2str(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bae8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::dec2str)();
  return;
}




/* Função: c2pstr | Endereço: 1000baec */

void .glue::c2pstr(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bb00. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::c2pstr)();
  return;
}




/* Função: SFPutFile | Endereço: 1000bb04 */

void .glue::SFPutFile(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bb18. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::SFPutFile)();
  return;
}




/* Função: Create | Endereço: 1000bb1c */

void .glue::Create(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bb30. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::Create)();
  return;
}




/* Função: FSDelete | Endereço: 1000bb34 */

void .glue::FSDelete(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bb48. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::FSDelete)();
  return;
}




/* Função: FSWrite | Endereço: 1000bb4c */

void .glue::FSWrite(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bb60. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::FSWrite)();
  return;
}




/* Função: log | Endereço: 1000bb64 */

double .glue::log(double __x)

{
  double dVar1;
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bb78. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  dVar1 = (double)(**(code **).TVect::log)();
  return dVar1;
}




/* Função: NewHandle | Endereço: 1000bb7c */

void .glue::NewHandle(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bb90. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::NewHandle)();
  return;
}




/* Função: GetHandleSize | Endereço: 1000bb94 */

void .glue::GetHandleSize(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bba8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::GetHandleSize)();
  return;
}




/* Função: cos | Endereço: 1000bbac */

double .glue::cos(double __x)

{
  double dVar1;
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bbc0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  dVar1 = (double)(**(code **).TVect::cos)();
  return dVar1;
}




/* Função: NewPtrClear | Endereço: 1000bbc4 */

void .glue::NewPtrClear(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bbd8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::NewPtrClear)();
  return;
}




/* Função: GetNextEvent | Endereço: 1000bbdc */

void .glue::GetNextEvent(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bbf0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::GetNextEvent)();
  return;
}




/* Função: GetDateTime | Endereço: 1000bbf4 */

void .glue::GetDateTime(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bc08. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::GetDateTime)();
  return;
}




/* Função: Random | Endereço: 1000bc0c */

void .glue::Random(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bc20. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::Random)();
  return;
}




/* Função: exp | Endereço: 1000bc24 */

void .glue::exp(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bc38. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::exp)();
  return;
}




/* Função: ceil | Endereço: 1000bc3c */

void .glue::ceil(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bc50. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::ceil)();
  return;
}




/* Função: MemError | Endereço: 1000bc54 */

void .glue::MemError(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bc68. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::MemError)();
  return;
}




/* Função: DisposePtr | Endereço: 1000bc6c */

void .glue::DisposePtr(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bc80. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::DisposePtr)();
  return;
}




/* Função: HLock | Endereço: 1000bc84 */

void .glue::HLock(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bc98. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::HLock)();
  return;
}




/* Função: HUnlock | Endereço: 1000bc9c */

void .glue::HUnlock(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bcb0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::HUnlock)();
  return;
}




/* Função: sqrt | Endereço: 1000bcb4 */

void .glue::sqrt(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bcc8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::sqrt)();
  return;
}




/* Função: GetEOF | Endereço: 1000bccc */

void .glue::GetEOF(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bce0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::GetEOF)();
  return;
}




/* Função: SetEOF | Endereço: 1000bce4 */

void .glue::SetEOF(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bcf8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::SetEOF)();
  return;
}




/* Função: GetGrayRgn | Endereço: 1000bcfc */

void .glue::GetGrayRgn(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bd10. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::GetGrayRgn)();
  return;
}




/* Função: NumToString | Endereço: 1000bd14 */

void .glue::NumToString(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bd28. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::NumToString)();
  return;
}




/* Função: SysBeep | Endereço: 1000bd2c */

void .glue::SysBeep(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bd40. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::SysBeep)();
  return;
}




/* Função: GlobalToLocal | Endereço: 1000bd44 */

void .glue::GlobalToLocal(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bd58. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::GlobalToLocal)();
  return;
}




/* Função: SystemClick | Endereço: 1000bd5c */

void .glue::SystemClick(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bd70. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::SystemClick)();
  return;
}




/* Função: SetRect | Endereço: 1000bd74 */

void .glue::SetRect(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bd88. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::SetRect)();
  return;
}




/* Função: BeginUpdate | Endereço: 1000bd8c */

void .glue::BeginUpdate(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bda0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::BeginUpdate)();
  return;
}




/* Função: EndUpdate | Endereço: 1000bda4 */

void .glue::EndUpdate(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bdb8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::EndUpdate)();
  return;
}




/* Função: DisposeHandle | Endereço: 1000bdbc */

void .glue::DisposeHandle(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bdd0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::DisposeHandle)();
  return;
}




/* Função: ExitToShell | Endereço: 1000bdd4 */

void .glue::ExitToShell(void)

{
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bde8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **).TVect::ExitToShell)();
  return;
}




/* Função: FUN_1000bdec | Endereço: 1000bdec */

void FUN_1000bdec(void)

{
  undefined4 *in_r12;
  undefined **ppuStack00000014;
  
  ppuStack00000014 = &.TVect::InitGraf;
                    /* WARNING: Could not recover jumptable at 0x1000bdfc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)*in_r12)();
  return;
}


