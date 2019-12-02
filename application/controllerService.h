#ifndef CONTROLLER_SERVICE
#define CONTROLLER_SERVICE

typedef struct conductor Conductor;
typedef struct tui_Props TUIRenderState;

int createControllerService(Conductor* conductor, int trainServer1, int trainServer2, TUIRenderState* prop);

#endif
