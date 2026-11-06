#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

//#define MJ_DESPERATE 
//#define _AUTOPALY

//////////////////////////////////////////////////////////////////////////
//״̬����
#define GS_FREE_FAKE		200			//αfree״̬ ������齫)

//��ֵ����
#define MAX_INDEX					35									//�������		//LIANG
#define MAX_COUNT					14									//�����Ŀ
#define	MASK_COLOR					0xF0								//��ɫ����
#define	MASK_VALUE					0x0F								//��ֵ����

//�������
#define ACK_NULL					0x00								//û������
#define ACK_LEFT					0x01								//�������
#define ACK_CENTER					0x02								//�г�����
#define ACK_RIGHT					0x04								//�ҳ�����
#define ACK_PENG					0x08								//��������
#define ACK_GANG					0x10								//��������
#define ACK_CHIHU					0x20								//�Ժ�����
//��������
#ifdef MJ_DESPERATE

const LONG SCORE_ADD_COMMON				= 6;							//�ӷ� ƽ  ��
const LONG SCORE_ADD_3A2				= 0;							//�ӷ� 3 �� 2
const LONG SCORE_ADD_7TS				= 5;							//�ӷ� ��С��
const LONG SCORE_ADD_13A				= 5;							//�ӷ� 13����
const LONG SCORE_ADD_W7T				= 7;							//�ӷ� ���߶�

const LONG SCORE_TIME_ZIMO				= 1;							//�ӷ� ��  ��
const LONG SCORE_ADD_TIANDI				= 10;							//�ӷ� ��غ�
const LONG SCORE_ADD_KING_WAIT			= 5;							//�ӷ� �׻���
const LONG SCORE_ADD_GANG_FLOWER		= 5;							//�ӷ� ���ϻ�
const LONG SCORE_ADD_WHOLE_COLOR		= 5;							//�ӷ� ��һɫ
const LONG SCORE_ADD_ALL_ASK			= 5;							//�ӷ� ȫ����
const LONG SCORE_ADD_7WINDS				= 1;							//�ӷ� �߷���

const LONG SCORE_ADD_NO_KING			= 2;							//�ӷ� �޲���
const LONG SCORE_ADD_PER_KING			= 1;							//�ӷ� �в���
const LONG SCORE_ADD_PER_BANKER			= 2;							//�ӷ� ��  ׯ

const LONG AFFORD_ADD_ACCEPT3			= 0;							//�ӷ� ��  ��
const LONG AFFORD_TIME_ACCEPT3			= 0;							//�ӷ� ��  ��
const LONG AFFORD_ADD_GIVE3				= 0;							//�ӷ� ��  ��
const LONG AFFORD_TIME_GIVE3			= 0;							//�ӷ� ��  ��
const LONG AFFORD_ADD_GANG_APPEND		= 5;							//�ӷ� ���ϻ�
const LONG AFFORD_TIME_GANG_APPEND		= 1;							//�ӷ� ���ϻ�
const LONG AFFORD_ADD_HIDE_WIND			= 5;							//�ӷ� ��  ��
const LONG AFFORD_TIME_HIDE_WIND		= 0;							//�ӷ� ��  ��

#else

const LONG SCORE_ADD_COMMON				= 1;							//�ӷ� ƽ  ��
const LONG SCORE_ADD_3A2				= 0;							//�ӷ� 3 �� 2
const LONG SCORE_ADD_7TS				= 2;							//�ӷ� ��С��
const LONG SCORE_ADD_13A				= 2;							//�ӷ� 13����
const LONG SCORE_ADD_W7T				= 4;							//�ӷ� ���߶�

const LONG SCORE_TIME_ZIMO				= 1;							//�ӷ� ��  ��
const LONG SCORE_ADD_TIANDI				= 4;							//�ӷ� ��غ�
const LONG SCORE_ADD_KING_WAIT			= 2;							//�ӷ� �׻���
const LONG SCORE_ADD_GANG_FLOWER		= 2;							//�ӷ� ���ϻ�
const LONG SCORE_ADD_WHOLE_COLOR		= 4;							//�ӷ� ��һɫ
const LONG SCORE_ADD_ALL_ASK			= 2;							//�ӷ� ȫ����
const LONG SCORE_ADD_7WINDS				= 2;							//�ӷ� �߷���

const LONG SCORE_ADD_NO_KING			= 2;							//�ӷ� �޲���
const LONG SCORE_ADD_PER_KING			= 1;							//�ӷ� �в���
const LONG SCORE_ADD_PER_BANKER			= 2;							//�ӷ� ��  ׯ

const LONG AFFORD_ADD_ACCEPT3			= 0;							//�ӷ� ��  ��
const LONG AFFORD_TIME_ACCEPT3			= 0;							//�ӷ� ��  ��
const LONG AFFORD_ADD_GIVE3				= 0;							//�ӷ� ��  ��
const LONG AFFORD_TIME_GIVE3			= 1;							//�ӷ� ��  ��
const LONG AFFORD_ADD_GANG_APPEND		= 0;							//�ӷ� ���ϻ�
const LONG AFFORD_TIME_GANG_APPEND		= 0;							//�ӷ� ���ϻ�
const LONG AFFORD_ADD_HIDE_WIND			= 0;							//�ӷ� ��  ��
const LONG AFFORD_TIME_HIDE_WIND		= 0;							//�ӷ� ��  ��

#endif

//����
const int MAX_AFFORD_COUNT				= 1;

//////////////////////////////////////////////////////////////////////////


//��������
enum EN_HU_CARD_QUEUE_TYPE
{
	EN_HU_CARD_QUEUE_NULL				= 0X0000,							//û�к�
	EN_HU_CARD_QUEUE_3A2				= 0X0001,							//�� 3��2
	EN_HU_CARD_QUEUE_7TS				= 0X0002,							//�� 7��
	EN_HU_CARD_QUEUE_13A				= 0X0004,							//�� 13��
	EN_HU_CARD_QUEUE_W7T				= 0X0008,							//�� ��7��
	EN_HU_CARD_QUEUE_WW					= 0X0010							//ȫ����
};
//���Ӻ�
enum EN_HU_ATTACHED_TYPE
{
	EN_HU_ATTACHED_NULL					= 0X0000,						//û�и��Ӻ�
	EN_HU_ATTACHED_ZIMO					= 0X0001,						//���� ����
	EN_HU_ATTACHED_TIANDI				= 0X0002,						//���� ��غ�
	EN_HU_ATTACHED_KING_WAIT			= 0X0004,						//���� ���Ƶ������׻��飩
	EN_HU_ATTACHED_GANG_FLOWER			= 0X0008,						//���� ���ϻ�
	EN_HU_ATTACHED_WHOLE_COLOR			= 0X0010,						//���� ��һɫ
	EN_HU_ATTACHED_ALL_ASK				= 0X0020,						//���� ȫ����
	EN_HU_ATTACHED_7WINDS				= 0X0040,						//���� �߷���
};

enum EN_AFFORD_TYPE
{
	EN_AFFORD_NULL						= 0,							//�ް���
	EN_AFFORD_ACCEPT3,													//��3����
	EN_AFFORD_GIVE3,													//��3����
	EN_AFFORD_GANG_APPEND,												//�ܲ�����
	EN_AFFORD_HIDE_WIND													//�ط����
};
//��������
struct tagAffordItem
{
	BYTE			cbAffordType;
	WORD			wChair;
	tagAffordItem()
	{
		cbAffordType = EN_AFFORD_NULL;
		wChair = INVALID_CHAIR;
	}
};

struct tagHuCourt
{
	BYTE			cbHuCardData;										//����	
	BYTE			cbCardEye;											//����
	BYTE			cbKingCount;										//������Ŀ						
	WORD			wHuCardQueueType;									//����������
	WORD			wHuAttachedType;									//���Ӻ�
	BYTE			cbBankerCount;										//��ׯ��Ŀ
	WORD			wWinChair;											//Ӯ  ��
	WORD			wFuseChair;											//������
	tagAffordItem	AffordItem[MAX_AFFORD_COUNT];						//����

	tagHuCourt()
	{
		Reset();
	}
	void Reset()
	{
		cbHuCardData		= 0;
		cbCardEye			= 0;
		cbKingCount			= 0;
		wHuCardQueueType	= EN_HU_CARD_QUEUE_NULL;
		wHuAttachedType		= EN_HU_ATTACHED_NULL;
		cbBankerCount		= 0;
		wWinChair			= INVALID_CHAIR;
		wFuseChair			= INVALID_CHAIR;
		AffordItem[0].wChair = INVALID_CHAIR;
		AffordItem[0].cbAffordType = EN_AFFORD_NULL;
	}
	bool IsHu()
	{
		return (wHuCardQueueType != EN_HU_CARD_QUEUE_NULL);
	}
	bool IsZimo()
	{
		return (IsHu() && (wHuAttachedType & EN_HU_ATTACHED_ZIMO));
	}
};


//////////////////////////////////////////////////////////////////////////

//��������
struct tagKindItem
{
	BYTE							cbWeaveKind;						//�������
	BYTE							cbCenterCard;						//�����˿�
	BYTE							cbCardIndex[3];						//�˿�����
};

//�������
struct tagWeaveItem
{
	BYTE							cbWeaveKind;						//�������
	BYTE							cbCenterCard;						//�����˿�
	WORD							wProvideUser;						//��Ӧ�û�
};

//��������
struct tagAnalyseItem
{
	BYTE							cbCardEye;							//�����˿�
	BYTE							cbWeaveKind[4];						//�������
	BYTE							cbCenterCard[4];					//�����˿�
};

//������Ϣ
struct tagGangInfo
{
	BYTE							cbGangCardData;						//����
	WORD							wPerformUser;						//������
	WORD							wProvideUser;						//�ṩ��
	BYTE							cbSendData;							//����
	BYTE							cbCurrentOutCount;					//��ʱ������Ŀ

	tagGangInfo()
	{
		Reset();
	}
	void Reset()
	{
		cbGangCardData			= 0;
		wPerformUser			= INVALID_CHAIR;
		wProvideUser			= INVALID_CHAIR;
		cbSendData				= 0;	
		cbCurrentOutCount		= 0XFF;
	}
};

//��������
struct tagHuInput
{
	BYTE*							pCardIndexSeq;						//����
	bool							bInsertCurrentCard;					//�Ƿ�����ж�	
	bool							bJudgeHuOnly;						//ֻ�жϺ�
	bool							bJudgeKingWait;						//�ж��׻���
	tagWeaveItem*					pWeaveItemSeq;						//�����
	BYTE							cbWeaveItemCount;					//�����Ŀ
	BYTE							cbCurrentCard;						//��ǰ��
	tagHuInput()
	{
		pCardIndexSeq		= 0;
		cbWeaveItemCount	= 0;
		bJudgeHuOnly		= true;
		bJudgeKingWait		= false;
		bInsertCurrentCard	= false;
	}
	bool IsValid()
	{
		return (pCardIndexSeq != 0 && pWeaveItemSeq != 0);
	}
};
//////////////////////////////////////////////////////////////////////////

//����˵��
typedef CArrayTemplate<tagAnalyseItem,tagAnalyseItem &> CAnalyseItemArray;

//��������
const BYTE CARD_KING_INDEX			=		MAX_INDEX-1;					//��������
const BYTE CARD_KING_DATA_MASK		=		0x80;							//���������ɰ�

//��Ϸ�߼���
class CGameLogic
{
	//��������
protected:
	static const BYTE				m_cbCardDataArray[136];					//�˿�����
	BYTE							m_cbKingCardData;						//����

	//��������
public:
	//���캯��
	CGameLogic();
	//��������
	virtual ~CGameLogic();

	//���ƺ���
public:
	//�����˿�
	void RandCardData(BYTE cbCardData[], BYTE bMaxCount);
	//ɾ���˿�
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard);
	//ɾ���˿�
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard[], BYTE bRemoveCount);
	//ɾ���˿�
	bool RemoveCard(BYTE cbCardData[], BYTE bCardCount, BYTE cbRemoveCard[], BYTE bRemoveCount);
	//��������
	void SetKingCardData(BYTE cbKingCardData);
	//��ȡ����
	BYTE GetKingCardData() { return m_cbKingCardData; }

	//��������
public:
	//��Ч�ж�
	bool IsValidCommonCard(BYTE cbCardData);
	//�ж�����
	bool IsKingCardData(BYTE cbCardData);
	bool IsKingCardIndex(BYTE cbCardIndex);
	//�˿���Ŀ
	BYTE GetCardCount(BYTE cbCardIndex[MAX_INDEX]);
	//��ȡ���
	BYTE GetWeaveCardData(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4]);
	//�Ƿ�ȫ3
	bool IsNeat3(BYTE cbCardIndex[MAX_INDEX]);
	//�Ƿ�ȫ2
	bool IsNeat2(BYTE cbCardIndex[MAX_INDEX]);
	//�Ƿ�ȫ��
	bool IsNeatAlone(BYTE cbCardIndex[MAX_INDEX]);
	//�Ƿ����
	bool IsWindCard(BYTE cbCardData) { return IsValidCommonCard(cbCardData) && ((cbCardData & MASK_COLOR) == 0X30);}
	
	//��������
public:
	//��������
	BYTE GetEatCardKind(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//��������
	BYTE GetPengCardKind(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//��������
	BYTE GetGangCardKind(BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//������Ϣ
	bool GetHuInfo(tagHuInput* pInput, tagHuCourt& HuCourt);
	//�������
	bool CalScore(tagHuCourt& HuCourt, LONG& lGain, LONG& lTimes);

	//��������
public:
	//����״̬
	bool IsTingPaiStatus(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE bItemCount);
	//������Ϣ
	BYTE GetGangCardInfo(BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE bItemCount, BYTE cbGangCard[4]);

	//ת������
public:
	//�˿�ת��
	BYTE SwitchToCardData(BYTE cbCardIndex);
	//�˿�ת��
	BYTE SwitchToCardIndex(BYTE cbCardData);
	//�˿�ת��
	BYTE SwitchToCardData(BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[], BYTE bMaxCount);
	//�˿�ת��
	BYTE SwitchToCardIndex(BYTE cbCardData[], BYTE bCardCount, BYTE cbCardIndex[MAX_INDEX]);
	//�������
	BYTE GetKingFromBrother(BYTE cbBrotherData);
	//�������ĵܵ�
	BYTE GetBrotherFromKing(BYTE cbKingData);

private:
	//�жϺ�
	bool _GetHuInfo			(tagHuInput* pInput, tagHuCourt& HuCourt, BYTE cbCardEye);
	bool _GetHuInfo3A2or7TS	(tagHuInput* pInput, int iInserKingtStart, bool bHaveType[4], tagHuCourt& HuCourt);
	bool __GetHuInfo3A2or7TS(tagHuInput* pInput, tagHuCourt& HuCourt);
	//ʮ������
	bool _GetHuInfo13A		(tagHuInput* pInput, tagHuCourt& HuCourt, BYTE cbCardEye);
	//ȫ����
	bool _GetHuInfoWW		(tagHuInput* pInput, tagHuCourt& HuCourt, BYTE cbCardEye);
	//�жϸ��Ӻ�
	bool _GetHuInfoAttached	(tagHuInput* pInput, tagHuCourt& HuCourt);
	//�����������
	void _AdjustHuInfo		(tagHuInput* pInput, tagHuCourt& HuCourt);

};

//////////////////////////////////////////////////////////////////////////

#endif