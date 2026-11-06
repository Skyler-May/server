#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//�˿�����
const BYTE CGameLogic::m_cbCardDataArray[136]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//����

	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//����

	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//ͬ��

	0x31,0x32,0x33,0x34,0x35,0x36,0x37,									//����
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,									//����
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,									//����
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,									//����
};

//BYTE CGameLogic::m_cbKingCardData	= 0xff;
//////////////////////////////////////////////////////////////////////////

//���캯��
CGameLogic::CGameLogic()
{
}

//��������
CGameLogic::~CGameLogic()
{
}

//�����˿�
void CGameLogic::RandCardData(BYTE cbCardData[], BYTE bMaxCount)
{
	//����׼��
	BYTE cbCardDataTemp[CountArray(m_cbCardDataArray)];
	CopyMemory(cbCardDataTemp,m_cbCardDataArray,sizeof(m_cbCardDataArray));

	//�����˿�
	BYTE bRandCount=0,bPosition=0;
	do
	{
		bPosition=rand()%(bMaxCount-bRandCount);
		cbCardData[bRandCount++]=cbCardDataTemp[bPosition];
		cbCardDataTemp[bPosition]=cbCardDataTemp[bMaxCount-bRandCount];
	} while (bRandCount<bMaxCount);

	//�滻����
	if(m_cbKingCardData != 0XFF)
	{
		for(int i=0; i<136; i++)
		{
			if(cbCardData[i] == m_cbKingCardData)
			{
				cbCardData[i] = CARD_KING_DATA_MASK;
			}
		}
	}
}

//ɾ���˿�
bool CGameLogic::RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard)
{
	//Ч���˿�
	ASSERT(IsValidCommonCard(cbRemoveCard));
	ASSERT(cbCardIndex[SwitchToCardIndex(cbRemoveCard)]>0);

	//ɾ���˿�
	BYTE cbRemoveIndex=SwitchToCardIndex(cbRemoveCard);
	if (cbCardIndex[cbRemoveIndex]>0)
	{
		cbCardIndex[cbRemoveIndex]--;
		return true;
	}

	//ʧ��Ч��
	ASSERT(FALSE);

	return false;
}

//ɾ���˿�
bool CGameLogic::RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard[], BYTE bRemoveCount)
{
	//ɾ���˿�
	for (BYTE i=0;i<bRemoveCount;i++)
	{
		//Ч���˿�
		ASSERT(IsValidCommonCard(cbRemoveCard[i]));
		ASSERT(cbCardIndex[SwitchToCardIndex(cbRemoveCard[i])]>0);

		//ɾ���˿�
		BYTE cbRemoveIndex=SwitchToCardIndex(cbRemoveCard[i]);
		if (cbCardIndex[cbRemoveIndex]==0)
		{
			//�������
			ASSERT(FALSE);

			//��ԭɾ��
			for (BYTE j=0;j<i;j++) 
			{
				ASSERT(IsValidCommonCard(cbRemoveCard[j]));
				cbCardIndex[SwitchToCardIndex(cbRemoveCard[j])]++;
			}

			return false;
		}
		else 
		{
			//ɾ���˿�
			--cbCardIndex[cbRemoveIndex];
		}
	}

	return true;
}

//ɾ���˿�
bool CGameLogic::RemoveCard(BYTE cbCardData[], BYTE bCardCount, BYTE cbRemoveCard[], BYTE bRemoveCount)
{
	//��������
	ASSERT(bCardCount<=14);
	ASSERT(bRemoveCount<=bCardCount);

	//�������
	BYTE bDeleteCount=0,cbTempCardData[14];
	if (bCardCount>CountArray(cbTempCardData)) return false;
	CopyMemory(cbTempCardData,cbCardData,bCardCount*sizeof(cbCardData[0]));

	//�����˿�
	for (BYTE i=0;i<bRemoveCount;i++)
	{
		for (BYTE j=0;j<bCardCount;j++)
		{
			if (cbRemoveCard[i]==cbTempCardData[j])
			{
				bDeleteCount++;
				cbTempCardData[j]=0;
				break;
			}
		}
	}

	//�ɹ��ж�
	if (bDeleteCount!=bRemoveCount) 
	{
		ASSERT(FALSE);
		return false;
	}

	//�����˿�
	BYTE bCardPos=0;
	for (BYTE i=0;i<bCardCount;i++)
	{
		if (cbTempCardData[i]!=0) cbCardData[bCardPos++]=cbTempCardData[i];
	}

	return true;
}

//��Ч�ж�
bool CGameLogic::IsValidCommonCard(BYTE cbCardData)
{
	BYTE cbValue=(cbCardData&MASK_VALUE);
	BYTE cbColor=(cbCardData&MASK_COLOR)>>4;
	return (((cbValue>=1)&&(cbValue<=9)&&(cbColor<=2))||((cbValue>=1)&&(cbValue<=7)&&(cbColor==3)));
}


//�˿���Ŀ
BYTE CGameLogic::GetCardCount(BYTE cbCardIndex[MAX_INDEX])
{
	//��Ŀͳ��
	BYTE cbCount=0;
	for (BYTE i=0;i<MAX_INDEX;i++) cbCount+=cbCardIndex[i];

	return cbCount;
}

//��ȡ���
BYTE CGameLogic::GetWeaveCardData(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4])
{
	//����˿�
	switch (cbWeaveKind)
	{
	case ACK_LEFT:		//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard+1;
			cbCardBuffer[2]=cbCenterCard+2;
			

			return 3;
		}
	case ACK_RIGHT:		//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard-1;
			cbCardBuffer[2]=cbCenterCard-2;

			return 3;
		}
	case ACK_CENTER:	//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard-1;
			cbCardBuffer[2]=cbCenterCard+1;

			return 3;
		}
	case ACK_PENG:		//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard;

			return 3;
		}
	case ACK_GANG:		//���Ʋ���
		{
			//���ñ���
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard;
			cbCardBuffer[3]=cbCenterCard;

			return 4;
		}
	default:
		{
			ASSERT(FALSE);
		}
	}

	return 0;
}


//��������
BYTE CGameLogic::GetEatCardKind(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	if(IsKingCardData(cbCurrentCard))
	{
		return ACK_NULL;
	}
	//�����ж�
	if (cbCurrentCard>=0x31) 
	{
		return 0;
	}

	//��������
	BYTE cbExcursion[3] = {0,1,2};
	BYTE cbItemKind[3] = {ACK_LEFT,ACK_CENTER,ACK_RIGHT};

	//�����ж�
	BYTE bEatKind=0,cbFirstIndex=0;
	BYTE cbCurrentIndex = SwitchToCardIndex(cbCurrentCard);
	for (BYTE i=0;i<CountArray(cbItemKind);i++)
	{
		BYTE cbValueIndex = cbCurrentIndex%9;
		if ((cbValueIndex >= cbExcursion[i]) && ((cbValueIndex-cbExcursion[i]) <= 6))
		{
			//�����ж�
			cbFirstIndex = cbCurrentIndex-cbExcursion[i];
			if ((cbCurrentIndex != cbFirstIndex)&&(cbCardIndex[cbFirstIndex]==0)) 
			{
				continue;
			}
			if ((cbCurrentIndex != (cbFirstIndex+1)) && (cbCardIndex[cbFirstIndex+1] == 0)) 
			{
				continue;
			}
			if ((cbCurrentIndex!=(cbFirstIndex+2)) && (cbCardIndex[cbFirstIndex+2]==0)) 
			{
				continue;
			}

			//��������
			bEatKind |= cbItemKind[i];
		}
	}

	return bEatKind;
}

//��������
BYTE CGameLogic::GetPengCardKind(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	if(IsKingCardData(cbCurrentCard))
	{
		return ACK_NULL;
	}
	ASSERT(IsValidCommonCard(cbCurrentCard));
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]>=2)?ACK_PENG:ACK_NULL;
}

//��������
BYTE CGameLogic::GetGangCardKind(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	if(IsKingCardData(cbCurrentCard))
	{
		return ACK_NULL;
	}
	ASSERT(IsValidCommonCard(cbCurrentCard));
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]>=3)?ACK_GANG:ACK_NULL;
}

//����״̬
bool CGameLogic::IsTingPaiStatus(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE bItemCount)
{
	//ѭ������
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if (cbCardIndex[i]<4)
		{
			BYTE cbCardData=SwitchToCardData(i);
			//if (GetHuCardKind(cbCardIndex,WeaveItem,bItemCount,cbCardData)!=HCT_NULL) return true;
			tagHuInput HuInput;
			HuInput.bJudgeHuOnly		= true;
			HuInput.bJudgeKingWait		= false;
			HuInput.bInsertCurrentCard	= true;
			HuInput.cbCurrentCard		= cbCardData;
			HuInput.cbWeaveItemCount	= bItemCount;
			HuInput.pCardIndexSeq		= cbCardIndex;
			tagHuCourt HuCourt;
			if(CGameLogic::GetHuInfo(&HuInput, HuCourt))
			{
				return true;
			}
		}
	}

	return false;
}

//������Ϣ
BYTE CGameLogic::GetGangCardInfo(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE bItemCount, BYTE cbGangCard[4])
{
	//���ñ���
	BYTE cbGangCount=0;
	ZeroMemory(cbGangCard,sizeof(BYTE)*4);

	//���ظ���
	for (BYTE i=0;i<CARD_KING_INDEX;i++)
	{
		if (cbCardIndex[i]>=4) cbGangCard[cbGangCount++]=SwitchToCardData(i);
	}

	//��ϸ���
	for (BYTE i=0;i<bItemCount;i++)
	{
		if (WeaveItem[i].cbWeaveKind==ACK_PENG)
		{
			BYTE cbCenterIndex=SwitchToCardIndex(WeaveItem[i].cbCenterCard);
			if (cbCardIndex[cbCenterIndex]==1) cbGangCard[cbGangCount++]=WeaveItem[i].cbCenterCard;
		}
	}

	return cbGangCount;
}

//�˿�ת��
BYTE CGameLogic::SwitchToCardData(BYTE cbCardIndex)
{
	ASSERT(cbCardIndex<MAX_INDEX);
	//�ж�����
	if(IsKingCardIndex(cbCardIndex))
	{
		return CARD_KING_DATA_MASK;
	}

	return ((cbCardIndex/9)<<4)|(cbCardIndex%9+1);
}

//�˿�ת��
BYTE CGameLogic::SwitchToCardIndex(BYTE cbCardData)
{
	//�ж�����
	if(IsKingCardData(cbCardData))
	{
		return CARD_KING_INDEX;
	}

	ASSERT(IsValidCommonCard(cbCardData));
	return ((cbCardData&MASK_COLOR)>>4)*9 +(cbCardData&MASK_VALUE)-1;	
}

//�˿�ת��
BYTE CGameLogic::SwitchToCardData(BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[], BYTE bMaxCount)
{
	//ת���˿� ���Ʒŵ���ǰ��
	BYTE bPosition=0;
	if(cbCardIndex[CARD_KING_INDEX] > 0)
	{
		for (BYTE j=0;j<cbCardIndex[CARD_KING_INDEX];j++)
		{
			ASSERT(bPosition<bMaxCount);
			cbCardData[bPosition++]=SwitchToCardData(CARD_KING_INDEX);
		}
	}
	
	for (BYTE i=0;i<MAX_INDEX;i++) 
	{
		if(i == CARD_KING_INDEX)
		{
			continue;
		}
		if (cbCardIndex[i]!=0)
		{
			for (BYTE j=0;j<cbCardIndex[i];j++)
			{
				ASSERT(bPosition<bMaxCount);
				cbCardData[bPosition++]=SwitchToCardData(i);
			}
		}
	}

	return bPosition;
}

//�˿�ת��
BYTE CGameLogic::SwitchToCardIndex(BYTE cbCardData[], BYTE bCardCount, BYTE cbCardIndex[MAX_INDEX])
{
	//���ñ���
	ZeroMemory(cbCardIndex,sizeof(BYTE)*MAX_INDEX);

	//ת���˿�
	for (BYTE i=0;i<bCardCount;i++)	
	{
		cbCardIndex[SwitchToCardIndex(cbCardData[i])]++;
	}

	return bCardCount;
}

//��������
void CGameLogic::SetKingCardData(BYTE cbKingCardData)
{
	if(cbKingCardData != 0xff)
	{
		ASSERT(IsValidCommonCard(cbKingCardData));
	}
	
	m_cbKingCardData = cbKingCardData;

}

//�ж�����
bool CGameLogic::IsKingCardData(BYTE cbCardData)
{
	if(m_cbKingCardData == 0xff)
	{
		return false;
	}

	return (cbCardData == CARD_KING_DATA_MASK? true : false);
}

bool CGameLogic::IsKingCardIndex(BYTE cbCardIndex)
{
	return (cbCardIndex == CARD_KING_INDEX ? true : false);
}

bool CGameLogic::CalScore(tagHuCourt& HuCourt, LONG& lGain, LONG& lTimes)
{
	ASSERT(HuCourt.IsHu());
	if(HuCourt.IsHu() == false)
	{
		return false;
	}

	//ȫ���� ȫ��Ӯ�꣬���������
	if(HuCourt.wHuCardQueueType == EN_HU_CARD_QUEUE_WW)
	{
		return false;
	}

	lTimes = 0;
	lGain  = SCORE_ADD_COMMON;
	switch(HuCourt.wHuCardQueueType)
	{
	case EN_HU_CARD_QUEUE_3A2:
		lGain += SCORE_ADD_3A2;
		lTimes += 0;		
		break;
	case EN_HU_CARD_QUEUE_7TS:
		lGain += SCORE_ADD_7TS;
		lTimes += 0;		
		break;
	case EN_HU_CARD_QUEUE_13A:
		lGain += SCORE_ADD_13A;		
		lTimes += 0;
		break;
	case EN_HU_CARD_QUEUE_W7T:
		lGain += SCORE_ADD_W7T;		
		lTimes += 0;
		break;
	}
	//��������
//	if(HuCourt.cbHuCardQueueType)

	//���� ����
	if(HuCourt.wHuAttachedType & EN_HU_ATTACHED_ZIMO)
	{
		lGain += 0;	
		lTimes += SCORE_TIME_ZIMO;
	}
	//���� ��غ�
	if(HuCourt.wHuAttachedType & EN_HU_ATTACHED_TIANDI)
	{
		lGain += SCORE_ADD_TIANDI;
		lTimes += 0;
	}
	//���� ���Ƶ������׻��飩
	if(HuCourt.wHuAttachedType & EN_HU_ATTACHED_KING_WAIT)
	{
		lGain += SCORE_ADD_KING_WAIT;
		lTimes += 0;
	}
	//���� ���ϻ�
	if(HuCourt.wHuAttachedType & EN_HU_ATTACHED_GANG_FLOWER)
	{
		lGain += SCORE_ADD_GANG_FLOWER;
		lTimes += 0;
	}
	//���� ��һɫ
	if(HuCourt.wHuAttachedType & EN_HU_ATTACHED_WHOLE_COLOR)
	{
		lGain += SCORE_ADD_WHOLE_COLOR;
		lTimes += 0;
	}
	//���� ȫ����
	if(HuCourt.wHuAttachedType & EN_HU_ATTACHED_ALL_ASK)
	{
		lGain += SCORE_ADD_ALL_ASK;
		lTimes += 0;
	}
	//���� �߷���
	if(HuCourt.wHuAttachedType & EN_HU_ATTACHED_7WINDS)
	{
		lGain += SCORE_ADD_7WINDS;
		lTimes += 0;
	}	
	//���� ����
	if(HuCourt.cbKingCount == 0)
	{
		lGain += SCORE_ADD_NO_KING;
		lTimes += 0;
	}
	else
	{
		lGain += SCORE_ADD_PER_KING*HuCourt.cbKingCount;
		lTimes += 0;
	}

	return true;
}

bool CGameLogic::GetHuInfo(tagHuInput* pInput, tagHuCourt& HuCourt)
{
	//Ч������
	ASSERT(pInput != 0 && pInput->IsValid());
	if(pInput == 0 || pInput->IsValid() == false)
	{
		return false;
	}

	//��������
	BYTE cbCurrentCardIndex = 0XFF;
	tagHuInput HuInput = *pInput;
	tagHuCourt HuCourtTmp = HuCourt;
	BYTE cbCardIndexTmp[MAX_INDEX];
	HuInput.pCardIndexSeq = cbCardIndexTmp;
	CopyMemory(cbCardIndexTmp, pInput->pCardIndexSeq, sizeof(cbCardIndexTmp));
	
	//������
	if(pInput->cbCurrentCard != 0)
	{
		cbCurrentCardIndex = SwitchToCardIndex(pInput->cbCurrentCard);
	}
	if(pInput->bInsertCurrentCard)
	{
		ASSERT(pInput->cbCurrentCard != 0);		
		cbCardIndexTmp[cbCurrentCardIndex] ++;
	}

	//�жϺ�
	bool bKingWaitHu = false;
	bool bCommonHu = false;

	//�ж�ȫ����
	if(_GetHuInfoWW(&HuInput, HuCourtTmp, 0))
	{
		bCommonHu = true;
	}
	//�ж�ʮ������ �������н�
	else if(_GetHuInfo13A(&HuInput, HuCourtTmp, 0))
	{
		bCommonHu = true;
	}
	else
	{
		//���õ�ǰ������ �������׻��飩
		if(pInput->bJudgeHuOnly == false && pInput->bJudgeKingWait)
		{
			if(cbCardIndexTmp[CARD_KING_INDEX] > 0)
			{
				cbCardIndexTmp[CARD_KING_INDEX] --;
				if(cbCardIndexTmp[cbCurrentCardIndex] > 0) //��ǰ�ƿ���������
				{
					cbCardIndexTmp[cbCurrentCardIndex] --;
					bKingWaitHu = _GetHuInfo(&HuInput, HuCourtTmp, pInput->cbCurrentCard);
					cbCardIndexTmp[CARD_KING_INDEX] ++;
				}			
				cbCardIndexTmp[cbCurrentCardIndex] ++;
			}
		}

		if(bKingWaitHu == false)
		{
			bool bRet = false;
			bool bHaveCardExceptKing = false;
			//���Լ���������
			for(BYTE i=0; i<CARD_KING_INDEX; i++)
			{
				if(cbCardIndexTmp[i] > 0)
				{
					bHaveCardExceptKing = true;
					if(cbCardIndexTmp[i] > 1)
					{
						cbCardIndexTmp[i] -= 2;
						bRet = _GetHuInfo(&HuInput, HuCourtTmp, SwitchToCardData(i));
						cbCardIndexTmp[i] += 2;
					}
					else if(cbCardIndexTmp[CARD_KING_INDEX] > 0)
					{
						if(HuInput.bJudgeKingWait 
							&& bKingWaitHu == false
							&& i == cbCurrentCardIndex)
						{
							continue;
						}					
						cbCardIndexTmp[i] --;
						cbCardIndexTmp[CARD_KING_INDEX] --;
						bRet = _GetHuInfo(&HuInput, HuCourtTmp, SwitchToCardData(i));
						cbCardIndexTmp[i] ++;
						cbCardIndexTmp[CARD_KING_INDEX] ++;
					}

					if(bRet)
					{
						bCommonHu = true;
						break;		
					}
				}			
			}

			//���ʣ��ȫ������
			if(bCommonHu == false && bHaveCardExceptKing == false)
			{
				bCommonHu = true;
			}
		}	
	}	

	if(bKingWaitHu || bCommonHu)
	{
		if(pInput->bJudgeHuOnly)
		{
			return true;
		}
		//���Ӻ� ȫ�����ټ��㸽�Ӻ�
		if(HuCourtTmp.wHuCardQueueType != EN_HU_CARD_QUEUE_WW)
		{
			_GetHuInfoAttached(&HuInput, HuCourtTmp);
			if(bKingWaitHu)
			{
				HuCourtTmp.wHuAttachedType |= EN_HU_ATTACHED_KING_WAIT;
			}
		}
		
		_AdjustHuInfo(&HuInput, HuCourtTmp);
		HuCourt = HuCourtTmp;
		return true;
	}

	return false;
	
}

bool CGameLogic::_GetHuInfo(tagHuInput* pInput, tagHuCourt& HuCourt, BYTE cbCardEye)
{
	int i = 0;
	//Ч������
	ASSERT(pInput && pInput->IsValid());
	if(pInput == 0 || pInput->IsValid() == false)
	{
		return false;
	}

	//���13��
	//if(_GetHuInfo13A(pInput, HuCourt, cbCardEye))
	//{
	//	return true;
	//}

	//������л�ɫ������ж�Ч��
	//���������ƺ����ƣ����ƻ�ɫһ�� ��߼��Ч�ʲ��Դ�ʹ��һɫ�Ĳ���
	bool bHaveType[4] = {true,true,true, true};
	if (pInput->pCardIndexSeq[CARD_KING_INDEX] > 0)
	{
		ZeroMemory(bHaveType, sizeof(bHaveType));
		//�������
		bool bHaveCardExceptKing = false;
		for(BYTE i=0; i<CARD_KING_INDEX; i++)
		{
			if(pInput->pCardIndexSeq[i] > 0)
			{
				bHaveCardExceptKing = true;
				ASSERT(((SwitchToCardData(i)&MASK_COLOR) >> 4)<4);
				bHaveType[(SwitchToCardData(i)&MASK_COLOR) >> 4] = true;
				i = (i/9+1)*9-1;
			}
		}
		//��鵹��
		if(bHaveCardExceptKing == false) 
		{
			if(pInput->bJudgeHuOnly)
			{
				return true;
			}
			for(i=0; i<pInput->cbWeaveItemCount; i++)
			{
				ASSERT(((pInput->pWeaveItemSeq[i].cbCenterCard&MASK_COLOR) >> 4)<4);
				bHaveType[(pInput->pWeaveItemSeq[i].cbCenterCard & MASK_COLOR) >> 4] = true;
			}
		}
	}


	if(_GetHuInfo3A2or7TS(pInput, 0, bHaveType, HuCourt))
	{
		return true;
	}


	return false;
}
bool CGameLogic::_GetHuInfo3A2or7TS(tagHuInput* pInput, int iInserKingtStart, bool bHaveType[4], tagHuCourt& HuCourt)
{
	//������
	ASSERT(pInput != 0 && pInput->IsValid());
	if(pInput == 0 || pInput->IsValid() == false)
	{
		return false;
	}
	ASSERT(pInput->pCardIndexSeq[CARD_KING_INDEX] <= 4);

	//��������
	//�ϴβ���ֵ
	BYTE cbPreKingType = (SwitchToCardData(iInserKingtStart)&MASK_COLOR) >> 4;
	ASSERT(cbPreKingType < 4 );

	//���Ƶݹ���
	if (pInput->pCardIndexSeq[CARD_KING_INDEX] > 0)
	{
		bool bRet = false;
		pInput->pCardIndexSeq[CARD_KING_INDEX] --;
		//��������ѯ
		for(BYTE i=cbPreKingType; i<4; i++)
		{
			if(bHaveType[i])
			{
				int iStartIndex = 9*i;
				if(i == cbPreKingType)
				{
					iStartIndex = iInserKingtStart;
				}
				int iEndIndex = (iStartIndex/9+1)*9;
				if(i>2)
				{
					iEndIndex = (iStartIndex/9)*9+7;
				}
				for(int j=iStartIndex; j<iEndIndex; j++)
				{
					pInput->pCardIndexSeq[j] ++;
					bRet = _GetHuInfo3A2or7TS(pInput, j, bHaveType, HuCourt);
					pInput->pCardIndexSeq[j] --;
					//��鷵��ֵ
					if(bRet)
					{
						break;					
					}
				}
				if(bRet)
				{
					break;
				}
			}

		}
		pInput->pCardIndexSeq[CARD_KING_INDEX] ++;
		return bRet;
	}

	return __GetHuInfo3A2or7TS(pInput, HuCourt);
}

bool CGameLogic::__GetHuInfo3A2or7TS(tagHuInput* pInput, tagHuCourt& HuCourt)
{
	//Ч������
	ASSERT(pInput && pInput->IsValid());
	if(pInput == 0 || pInput->IsValid() == false)
	{
		return false;
	}

	//��ʱ����С�������ж�
	if(pInput->cbWeaveItemCount == 0 && IsNeat2(pInput->pCardIndexSeq))
	{
		HuCourt.wHuCardQueueType = EN_HU_CARD_QUEUE_7TS;
		return true;
	}
	if(IsNeat3(pInput->pCardIndexSeq))
	{
		HuCourt.wHuCardQueueType = EN_HU_CARD_QUEUE_3A2;
		return true;
	}
	
	return false;
}

bool CGameLogic::_GetHuInfo13A(tagHuInput* pInput, tagHuCourt& HuCourt, BYTE cbCardEye)
{
	//Ч������
	if(pInput == 0 || pInput->IsValid() == false /*|| cbCardEye == 0*/)
	{
		ASSERT(0);
		return false;
	}

	if(pInput->cbWeaveItemCount > 0)
	{
		return false;
	}
	
	bool bRet = false;
	//�����۲���
	//pInput->pCardIndexSeq[SwitchToCardIndex(cbCardEye)] ++;
	bRet = IsNeatAlone(pInput->pCardIndexSeq);
	if(bRet)
	{
		HuCourt.wHuCardQueueType = EN_HU_CARD_QUEUE_13A;
	}
	//pInput->pCardIndexSeq[SwitchToCardIndex(cbCardEye)] --;

	return bRet;
}

//ȫ����
bool CGameLogic::_GetHuInfoWW(tagHuInput* pInput, tagHuCourt& HuCourt, BYTE cbCardEye)
{
#ifndef MJ_DESPERATE	//��Ϫ�齫�޷�һɫ
	return false;
#endif
	//Ч������
	if(pInput == 0 || pInput->IsValid() == false /*|| cbCardEye == 0*/)
	{
		ASSERT(0);
		return false;
	}
	for(int i=0; i<pInput->cbWeaveItemCount; i++)
	{
		if(IsWindCard(pInput->pWeaveItemSeq[i].cbCenterCard) == false)
		{
			return false;
		}
	}

	for(int i=0; i<27; i++)
	{
		if(pInput->pCardIndexSeq[i] != 0)
		{
			return false;
		}
	}

	HuCourt.wHuCardQueueType = EN_HU_CARD_QUEUE_WW;

	return true;
}

//
bool CGameLogic::IsNeat3(BYTE cbCardIndex[MAX_INDEX])
{
	int i = 0;
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	//��������
	BYTE cbIndex[4]={0,1,2,3};
	tagKindItem * pKindItem[4];
	ZeroMemory(&pKindItem,sizeof(pKindItem));


	//�����ж�
	BYTE cbCardCount = 0;
	for(BYTE i=0; i<CARD_KING_INDEX; i++)
	{
		cbCardCount += cbCardIndexTemp[i];
	}
	//Ч����Ŀ
	ASSERT(cbCardCount<=14);
	if((cbCardCount%3) != 0)
	{
		return false;
	}

	BYTE bLessKindItem = cbCardCount/3;
	if (bLessKindItem == 0)
	{
		return true;
	}

	//�жϷ���
	BYTE cbWindCardIndex = 27;

	//��������
	BYTE bKindItemCount=0;
	tagKindItem KindItem[12];
	ZeroMemory(KindItem,sizeof(KindItem));

	//��ַ���
	if (cbCardCount>=3)
	{
		for (BYTE i=0;i<MAX_INDEX;i++)
		{
			//ͬ���ж�
			if (cbCardIndex[i]>=3)
			{
				KindItem[bKindItemCount].cbCenterCard=i;
				KindItem[bKindItemCount].cbCardIndex[0]=i;
				KindItem[bKindItemCount].cbCardIndex[1]=i;
				KindItem[bKindItemCount].cbCardIndex[2]=i;
				KindItem[bKindItemCount++].cbWeaveKind=ACK_PENG;
			}

			//�����ж�
			if ((i<(cbWindCardIndex-2)) && (cbCardIndex[i]>0) && ((i%9)<7))
			{
				for (BYTE j=1; j<=cbCardIndex[i]; j++)
				{
					if((cbCardIndex[i+1]>=j) && (cbCardIndex[i+2]>=j))
					{
						KindItem[bKindItemCount].cbCenterCard=i;
						KindItem[bKindItemCount].cbCardIndex[0]=i;
						KindItem[bKindItemCount].cbCardIndex[1]=i+1;
						KindItem[bKindItemCount].cbCardIndex[2]=i+2;
						KindItem[bKindItemCount++].cbWeaveKind=ACK_LEFT;
					}
				}
			}
		}
	}

	//��Ϸ���
	if (bKindItemCount>=bLessKindItem)
	{
		//��������
		BYTE cbCardIndexTemp[MAX_INDEX];
		ZeroMemory(cbCardIndexTemp,sizeof(cbCardIndexTemp));

		//��������
		BYTE cbIndex[4]={0,1,2,3};
		tagKindItem * pKindItem[4];
		ZeroMemory(&pKindItem,sizeof(pKindItem));

		//��ʼ���
		do
		{
			//���ñ���
			CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));
			for (BYTE i=0;i<bLessKindItem;i++) 
			{
				pKindItem[i]=&KindItem[cbIndex[i]];
			}

			//�����ж�
			bool bEnoughCard=true;
			for (BYTE i=0;i<bLessKindItem*3;i++)
			{
				//�����ж�
				BYTE cbCardIndex=pKindItem[i/3]->cbCardIndex[i%3];
				if (cbCardIndexTemp[cbCardIndex]==0)
				{
					bEnoughCard=false;
					break;
				}
				else 
				{
					cbCardIndexTemp[cbCardIndex]--;
				}
			}
			if(bEnoughCard)
			{
				return true;
			}

			//��������
			if (cbIndex[bLessKindItem-1] == (bKindItemCount-1))
			{
				for (BYTE i=bLessKindItem-1;i>0;i--)
				{
					if ((cbIndex[i-1]+1) != cbIndex[i])
					{
						BYTE cbNewIndex = cbIndex[i-1];
						for (BYTE j=(i-1); j<bLessKindItem; j++) 
						{
							cbIndex[j]=cbNewIndex+j-i+2;
						}
						break;
					}
				}
				if (i==0)
				{
					break;
				}
			}
			else 
			{
				cbIndex[bLessKindItem-1]++;
			}

		} while (true);

	}

	return false;
}

bool CGameLogic::IsNeat2(BYTE cbCardIndex[MAX_INDEX])
{
	for(BYTE i=0; i<CARD_KING_INDEX; i++)
	{
		if(cbCardIndex[i] != 0 && cbCardIndex[i] != 2)
		{
			return false;
		}
	}

	return true;
}

bool CGameLogic::IsNeatAlone(BYTE cbCardIndex[MAX_INDEX])
{
	int i = 0;
	//��������
	const BYTE COMMON_TYPE_SUM = 9;
	for (BYTE i=0;i<CARD_KING_INDEX;i++) 
	{
		if(cbCardIndex[i] > 1)
		{
			return false;
		}
	}

	BYTE* pKingIndex = 0;
	for(i=0; i<3; i++)
	{
		pKingIndex = &(cbCardIndex[i*COMMON_TYPE_SUM]);
		for(BYTE j=0; j<COMMON_TYPE_SUM; j++)
		{
			if(pKingIndex[j] > 0)
			{
				for(BYTE k=0; k<2; k++)
				{
					j ++;
					if(j<COMMON_TYPE_SUM)
					{
						if(pKingIndex[j] > 0)
						{
							return false;
						}
					}
				}

			}
		}
	}

	return true;
}


bool CGameLogic::_GetHuInfoAttached(tagHuInput* pInput, tagHuCourt& HuCourt)
{
	int i = 0;
	//Ч������
	ASSERT(pInput && pInput->IsValid());
	if(pInput == 0 || pInput->IsValid() == false)
	{
		return false;
	}

	//�ж���һɫ
	bool bWholeColor = true;
	BYTE cbCardType = 0XFF;
	for(BYTE i=0; i<pInput->cbWeaveItemCount; i++)
	{
		if(cbCardType == 0XFF)
		{
			cbCardType = (pInput->pWeaveItemSeq[i].cbCenterCard & MASK_COLOR);
		}
		else if(cbCardType != (pInput->pWeaveItemSeq[i].cbCenterCard & MASK_COLOR))
		{
			bWholeColor = false;
			break;
		}
	}
	if(bWholeColor)
	{
		for(i=0; i<CARD_KING_INDEX; i++)
		{
			if(pInput->pCardIndexSeq[i] > 0)
			{
				BYTE cbCardData = SwitchToCardData(i);
				if(cbCardType == 0XFF)
				{
					cbCardType = (cbCardData& MASK_COLOR);
				}
				else if(cbCardType != (cbCardData & MASK_COLOR))
				{
					bWholeColor = false;
					break;
				}
			}
		}
	}
	if(bWholeColor)
	{
		HuCourt.wHuAttachedType |= EN_HU_ATTACHED_WHOLE_COLOR;
	}

	//�ж�ȫ����
	if(pInput->cbWeaveItemCount == 4)
	{
		HuCourt.wHuAttachedType |= EN_HU_ATTACHED_ALL_ASK;
	}

	//�ж�������Ŀ
	HuCourt.cbKingCount = pInput->pCardIndexSeq[CARD_KING_INDEX];

	//�ж��߷��
	if(HuCourt.wHuCardQueueType == EN_HU_CARD_QUEUE_13A)
	{
		bool bWindsPresent = true;
		for(int i=27; i<(27+7); i++)
		{
			if(pInput->pCardIndexSeq[i] == 0)
			{
				bWindsPresent = false;
				break;
			}
		}
		if(bWindsPresent)
		{
			HuCourt.wHuAttachedType |= EN_HU_ATTACHED_7WINDS;
		}
	}

	return true;
}

//�����������
void CGameLogic::_AdjustHuInfo(tagHuInput* pInput, tagHuCourt& HuCourt)
{
	//Ч������
	ASSERT(pInput && pInput->IsValid());
	if(pInput == 0 || pInput->IsValid() == false)
	{
		return;
	}

	//�޲������С��
	if(HuCourt.wHuCardQueueType == EN_HU_CARD_QUEUE_7TS
		&& HuCourt.cbKingCount == 0)
	{
		HuCourt.wHuCardQueueType = EN_HU_CARD_QUEUE_W7T;
	}
}


//�������
BYTE CGameLogic::GetKingFromBrother(BYTE cbBrotherData)
{
	if(IsValidCommonCard(cbBrotherData) == false)
	{
		return 0XFF;
	}
	BYTE cbColor = cbBrotherData & MASK_COLOR;
	BYTE cbValue = (cbBrotherData & MASK_VALUE) + 1;
	if(cbColor != 0X30)
	{
		if(cbValue > 0X09)
		{
			cbValue = 0X01;
		}
	}
	else if(cbValue < 0X06)
	{
		if(cbValue > 0X04)
		{
			cbValue = 0X01;
		}
	}
	else
	{
		if(cbValue > 0X07)
		{
			cbValue = 0X05;
		}
	}

	return (cbColor | cbValue);
}


//�������ĵܵ�
BYTE CGameLogic::GetBrotherFromKing(BYTE cbKingData)
{
	if(IsValidCommonCard(cbKingData) == false)
	{
		return 0XFF;
	}
	BYTE cbColor = cbKingData & MASK_COLOR;
	BYTE cbValue = (cbKingData & MASK_VALUE) - 1;
	if(cbColor != 0X30)
	{
		if(cbValue < 0X01)
		{
			cbValue = 0X09;
		}
	}
	else if(cbValue < 0X04)
	{
		if(cbValue < 0X01)
		{
			cbValue = 0X04;
		}
	}
	else
	{
		if(cbValue < 0X05)
		{
			cbValue = 0X07;
		}
	}

	return (cbColor | cbValue);
}
//////////////////////////////////////////////////////////////////////////





















