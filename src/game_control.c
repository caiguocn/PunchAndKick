#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_WIDGET_H
#include LC_ACTIVEBOX_H

#include "game.h"

GamePlayer player_data[4];

/** �����ɫ���ƶ�����������Դ */
static ActionData* ActionRes_LoadWalk(void)
{
	int i;
	ActionData *action;
	LCUI_Graph img_move[8];
	char path[512];

	action = Action_Create();
	for(i=0; i<8; ++i) {
		Graph_Init( &img_move[i] );
		sprintf( path, "drawable/walk-%02d.png", i+1 );
		Graph_LoadImage( path, &img_move[i] );
		Action_AddFrame( action, -6,0, &img_move[i], 100 );
	}
	return action;
}

/** �����ɫ��վ������������Դ */
static ActionData* ActionRes_LoadStance(void)
{
	int i;
	ActionData *action;
	LCUI_Graph img_stance[4];
	char path[512];
	
	action = Action_Create();
	for(i=0; i<4; ++i) {
		Graph_Init( &img_stance[i] );
		sprintf( path, "drawable/stance-%02d.png", i+1 );
		Graph_LoadImage( path, &img_stance[i] );
		Action_AddFrame( action, -5,0, &img_stance[i], 100 );
	}
	return action;
}

/** �����ɫ�Ķ���������Դ */
ActionData* LoadGamePlayerRes( int action_type )
{
	switch( action_type ) {
	case STATE_STANCE: return ActionRes_LoadStance();
	case STATE_WALK: return ActionRes_LoadWalk();
	}
	return NULL;
}

int GamePlayer_InitAction( GamePlayer *player )
{
	ActionData* action;

	player->state = STATE_STANCE;
	/* ����GameObject���� */
	player->object = GameObject_New();
	/* ������Ϸ��ɫ��Դ */
	action = LoadGamePlayerRes( STATE_STANCE );
	/* ����������������Ϸ���� */
	GameObject_AddAction( player->object, action, STATE_STANCE );
	
	action = LoadGamePlayerRes( STATE_WALK );
	GameObject_AddAction( player->object, action, STATE_WALK );

	/* �ƶ���Ϸ��ɫ��λ�� */
	GameObject_Move( player->object, 300, 300 );
	//Widget_SetBorder( player->object, Border(1,BORDER_STYLE_SOLID, RGB(0,0,0)) );
	Widget_Show( player->object );
	return 0;
}

int Game_Init(void)
{
	int ret;
	/* ע��GameObject���� */
	GameObject_Register();
	/* ��¼���ID */
	player_data[0].id = 1;
	player_data[1].id = 2;
	player_data[2].id = 3;
	player_data[3].id = 4;
	/* ���û������� */
	player_data[0].walk_speed = 100;
	player_data[0].run_speed = 100;
	player_data[0].human_control = TRUE;
	player_data[0].local_control = TRUE;
	player_data[0].right_direction = TRUE;
	/* ��ʼ����ɫ�������� */
	ret = GamePlayer_InitAction( &player_data[0] );
	//Game_InitPlayerAction( &player_data[1] );
	//Game_InitPlayerAction( &player_data[2] );
	//Game_InitPlayerAction( &player_data[3] );
	/* ��Ӧ�������� */
	ret |= LCUI_KeyboardEvent_Connect( GameKeyboardProc, NULL );
	ret |= GameMsgLoopStart();
	return ret;
}

static void GamePlayer_ProcState( GamePlayer *player, int state )
{
	double n;
	int x, y;

	switch( state ) {
	case STATE_WALK:
		n = 1.0 * player->walk_speed;
		n = n*2 / 100;
		GameObject_GetPos( player->object, &x, &y );
		if( player->right_direction ) {
			x += (int)n;
		} else {
			x -= (int)n;
		}
		GameObject_Move( player->object, x, y );
	default:
		break;
	}
}

static void GamePlayer_Control( void *arg )
{
	int i;
	while(1) {
		/* �������Ϊ4�������ڻ����ڿ����׶Σ�����Ϊ1 */
		for(i=0; i<1; ++i) {
			if( !player_data[i].local_control ) {
				continue;
			}
			GamePlayer_ProcState( &player_data[i], player_data[i].state );
		}
		LCUI_MSleep( 10 );
	}
}

int Game_Start(void)
{
	LCUI_Thread t;
	LCUIThread_Create( &t, GamePlayer_Control, NULL );
	GameObject_PlayAction( player_data[0].object );
	//GameObject_PlayAction( player_data[1].object );
	//GameObject_PlayAction( player_data[2].object );
	//GameObject_PlayAction( player_data[3].object );
	return 0;
}

int Game_Pause(void)
{
	return 0;
}

/** ͨ�����Ƽ���ȡ�ü����ƵĽ�ɫ */
GamePlayer *GamePlayer_GetPlayerByControlKey( int key_code )
{
	return &player_data[0];
}

/** ͨ����ɫID����ȡ��ɫ */
GamePlayer *GamePlayer_GetByID( int player_id )
{
	if( player_id > 4 ) {
		return NULL;
	}
	return &player_data[player_id-1];
}

/** �ı��ɫ��״̬  */
void GamePlayer_ChangeState( GamePlayer *player, int state )
{
	GameMsg msg;

	switch(state) {
	case STATE_RUN:
	case STATE_STANCE:
	default:
		break;
	}
	/* ��״̬��һ�� */
	if( player->state != state ) {
		msg.player_id = player->id;
		msg.msg.msg_id = GAMEMSG_STATE;
		msg.msg.state.state = state;
		Game_PostMsg( &msg );
		player->state = state;
	}
}