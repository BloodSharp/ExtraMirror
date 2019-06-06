#include "main.h"
extern cvar_t *events_block;
pfnEventMsgHook pEvent_usp;
pfnEventMsgHook pEvent_glock;
pfnEventMsgHook pEvent_ak47;
pfnEventMsgHook pEvent_aug;
pfnEventMsgHook pEvent_awp;
pfnEventMsgHook pEvent_createexplo;
pfnEventMsgHook pEvent_deagle;
pfnEventMsgHook pEvent_elite_left;
pfnEventMsgHook pEvent_elite_right;
pfnEventMsgHook pEvent_famas;
pfnEventMsgHook pEvent_fiveseven;
pfnEventMsgHook pEvent_g3sg1;
pfnEventMsgHook pEvent_galil;
pfnEventMsgHook pEvent_m3;
pfnEventMsgHook pEvent_m4a1;
pfnEventMsgHook pEvent_m249;
pfnEventMsgHook pEvent_mac10;
pfnEventMsgHook pEvent_mp5n;
pfnEventMsgHook pEvent_p90;
pfnEventMsgHook pEvent_p228;
pfnEventMsgHook pEvent_sg550;
pfnEventMsgHook pEvent_sg552;
pfnEventMsgHook pEvent_tmp;
pfnEventMsgHook pEvent_ump45;
pfnEventMsgHook pEvent_scout;
pfnEventMsgHook pEvent_vehicle;
pfnEventMsgHook pEvent_xm1014;
pfnEventMsgHook pEvent_knife;
pfnEventMsgHook pEvent_createsmoke;

void Event_createsmoke(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_createsmoke(args); } }
void Event_knife(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_knife(args); } }
void Event_usp(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_usp(args); } }
void Event_ak47(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_ak47(args); } }
void Event_aug(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_aug(args); } }
void Event_awp(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_awp(args); } }
void Event_createexplo(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_createexplo(args); } }
void Event_deagle(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_deagle(args); } }
void Event_elite_left(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_elite_left(args); } }
void Event_elite_right(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_elite_right(args); } }
void Event_famas(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_famas(args); } }
void Event_fiveseven(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_fiveseven(args); } }
void Event_g3sg1(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_g3sg1(args); } }
void Event_galil(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_galil(args); } }
void Event_glock(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_glock(args); } }
void Event_m3(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_m3(args); } }
void Event_m4a1(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_m4a1(args); } }
void Event_m249(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_m249(args); } }
void Event_mac10(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_mac10(args); } }
void Event_mp5n(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_mp5n(args); } }
void Event_p90(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_p90(args); } }
void Event_p228(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_p228(args); } }
void Event_scout(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_scout(args); } }
void Event_sg550(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_sg550(args); } }
void Event_sg552(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_sg552(args); } }
void Event_tmp(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_tmp(args); } }
void Event_ump45(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_ump45(args); } }
void Event_vehicle(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_vehicle(args); } }
void Event_xm1014(struct event_args_s *args) { if (events_block->value > 0) {} else { pEvent_xm1014(args); } }
