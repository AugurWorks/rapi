#include "RApiPlus.h"

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef WinOS
#include <unistd.h>
#else
#include <Windows.h>
#endif

#define GOOD 0
#define BAD  1

using namespace std;
using namespace RApi;

/*   =====================================================================   */

bool g_bMdLoginComplete = false;

int main(int argc, char * * argv, char * * envp);

class MyAdmCallbacks: public AdmCallbacks {
  public :
  MyAdmCallbacks()  {};
  ~MyAdmCallbacks() {};

  virtual int Alert(AlertInfo * pInfo, void * pContext, int * aiCode);
};

class MyCallbacks: public RCallbacks {
  public :
  MyCallbacks()  {};
  ~MyCallbacks() {};

  virtual int Alert(AlertInfo * pInfo, void * pContext, int * aiCode);

  virtual int Bar(BarInfo * pInfo, void * pContext, int * aiCode);

  virtual int BarReplay(BarReplayInfo * pInfo, void * pContext, int * aiCode);

  private :
};

int MyAdmCallbacks::Alert(AlertInfo * pInfo, void * pContext, int * aiCode) {
  int iIgnored;

  if (!pInfo -> dump(&iIgnored)) {
    cout << "error in pInfo -> dump : " << iIgnored << endl;
  }

  *aiCode = API_OK;
  return (OK);
}

int MyCallbacks::Alert(AlertInfo * pInfo, void * pContext, int * aiCode) {
  int iIgnored;

  if (!pInfo -> dump(&iIgnored)) {
    cout << "error in pInfo -> dump : " << iIgnored << endl;
  }

  if (pInfo -> iAlertType == ALERT_LOGIN_COMPLETE && pInfo -> iConnectionId == MARKET_DATA_CONNECTION_ID) {
    cout << "Alerting" << endl;
    g_bMdLoginComplete = true;
  }

  *aiCode = API_OK;
  return (OK);
}

int MyCallbacks::Bar(BarInfo * pInfo, void * pContext, int * aiCode) {
  cout << "DATA " << pInfo -> dClosePrice << " " << pInfo -> iEndSsboe << endl;
  *aiCode = API_OK;
  return (OK);
}

int MyCallbacks::BarReplay(BarReplayInfo * pInfo, void * pContext, int * aiCode) {
  *aiCode = API_OK;
  return (OK);
}

int main(int argc, char * * argv, char * * envp) {
  char * USAGE = (char *)"SampleMD user password exchange ticker";

  REngine *        pEngine;
  MyAdmCallbacks * pAdmCallbacks;
  RCallbacks *     pCallbacks;
  REngineParams    oParams;
  LoginParams      oLoginParams;
  char *           fake_envp[10];
  int              iCode;

  if (argc < 5) {
    cout << USAGE << endl;
    return (BAD);
  }

  try {
    pAdmCallbacks = new MyAdmCallbacks();
  } catch (OmneException& oEx) {
    iCode = oEx.getErrorCode();
    cout << "MyAdmCallbacks::MyAdmCallbacks() error : " << iCode << endl;
    return (BAD);
  }

  /*   ----------------------------------------------------------------   */
  /*   The following fake envp contains the settings for connecting to    */
  /*   Rithmic 01 Test.  To connect to a different instance of the        */
  /*   Rithmic trading platform, consult appropriate connection params    */
  /*   document in your download directory.                               */

  fake_envp[0]  = "MML_DMN_SRVR_ADDR=rituz00100.00.rithmic.com:65000~rituz00100.00.rithmic.net:65000~rituz00100.00.theomne.net:65000~rituz00100.00.theomne.com:65000";
  fake_envp[1]  = "MML_DOMAIN_NAME=rithmic_uat_dmz_domain";
  fake_envp[2]  = "MML_LIC_SRVR_ADDR=rituz00100.00.rithmic.com:56000~rituz00100.00.rithmic.net:56000~rituz00100.00.theomne.net:56000~rituz00100.00.theomne.com:56000";
  fake_envp[3]  = "MML_LOC_BROK_ADDR=rituz00100.00.rithmic.com:64100";
  fake_envp[4]  = "MML_LOGGER_ADDR=rituz00100.00.rithmic.com:45454~rituz00100.00.rithmic.net:45454~rituz00100.00.theomne.com:45454~rituz00100.00.theomne.net:45454";
  fake_envp[5]  = "MML_LOG_TYPE=log_net";

  /*   The SSL files are located in the ./<version>/etc directory         */
  /*   of the R | API package.  The settings below assume that those      */
  /*   files are in the current working directory.  Normally you should   */
  /*   specify the full path to these files.                              */
  fake_envp[6]  = "MML_SSL_CLNT_AUTH_FILE=../etc/rithmic_ssl_cert_auth_params";
  fake_envp[7]  = "MML_SSL_CLNT_CERT_FILE=../etc/rithmic_ssl_client_params";
  fake_envp[8]  = "MML_SSL_CLNT_KEY_FILE=../etc/rithmic_ssl_client_private_key";

  fake_envp[9]  = "USER=drew@augurworks.com";

  /*   ----------------------------------------------------------------   */
  /*   The following value for REngineParams::sAdmCnnctPt assumes the     */
  /*   app is connecting to the Rithmic 01 Test environment.              */
  /*   REngineParams::sAppName must be changed in order to log into the   */
  /*   Rithmic Paper Trading environment, or any production environment.  */

  oParams.sAppName.pData        = "AugurWorks";
  oParams.sAppName.iDataLen     = (int)strlen(oParams.sAppName.pData);
  oParams.sAppVersion.pData     = "1.0.0.0";
  oParams.sAppVersion.iDataLen  = (int)strlen(oParams.sAppVersion.pData);
  oParams.sAdmCnnctPt.pData     = "dd_admin_sslc";
  oParams.sAdmCnnctPt.iDataLen  = (int)strlen(oParams.sAdmCnnctPt.pData);
  oParams.envp                  = fake_envp;
  oParams.pAdmCallbacks         = pAdmCallbacks;
  oParams.sLogFilePath.pData    = "smd.log";
  oParams.sLogFilePath.iDataLen = (int)strlen(oParams.sLogFilePath.pData);

  try {
    pEngine = new REngine(&oParams);
  } catch (OmneException& oEx) {
    delete pAdmCallbacks;

    iCode = oEx.getErrorCode();
    cout << oEx.getErrorString();
    cout << "REngine::REngine() error : " << iCode << endl;
    return (BAD);
  }

     /*   ----------------------------------------------------------------   */
     /*   instantiate a callback object - prerequisite for logging in */

  try {
    pCallbacks = new MyCallbacks();
  } catch (OmneException& oEx) {
    delete pEngine;
    delete pAdmCallbacks;

    iCode = oEx.getErrorCode();
    cout << "MyCallbacks::MyCallbacks() error : " << iCode << endl;
    return (BAD);
  }

  /*   ----------------------------------------------------------------   */
  /*   Set up parameters for logging in.  Again, the MdCnnctPt and        */
  /*   TsCnnctPt have values for Rithmic 01 Test.  Add values for other   */
  /*   members of LoginParams to log into other subsystems of the         */
  /*   infrastructure like like pnl and history.                          */

  oLoginParams.pCallbacks           = pCallbacks;

  oLoginParams.sMdUser.pData        = argv[1];
  oLoginParams.sMdUser.iDataLen     = (int)strlen(oLoginParams.sMdUser.pData);

  oLoginParams.sMdPassword.pData    = argv[2];
  oLoginParams.sMdPassword.iDataLen = (int)strlen(oLoginParams.sMdPassword.pData);

  oLoginParams.sMdCnnctPt.pData     = "login_agent_tpc";
  oLoginParams.sMdCnnctPt.iDataLen  = (int)strlen(oLoginParams.sMdCnnctPt.pData);

  oLoginParams.sIhCnnctPt.pData     = "login_agent_historyc";
  oLoginParams.sIhCnnctPt.iDataLen  = (int)strlen(oLoginParams.sIhCnnctPt.pData);

  oLoginParams.sTsCnnctPt.pData     = "login_agent_opc";
  oLoginParams.sTsCnnctPt.iDataLen  = (int)strlen(oLoginParams.sTsCnnctPt.pData);

  oLoginParams.sPnlCnnctPt.pData     = "login_agent_pnlc";
  oLoginParams.sPnlCnnctPt.iDataLen  = (int)strlen(oLoginParams.sPnlCnnctPt.pData);

  oLoginParams.sUser.pData          = argv[1];
  oLoginParams.sUser.iDataLen       = (int)strlen(oLoginParams.sUser.pData);

  oLoginParams.sPassword.pData      = argv[2];
  oLoginParams.sPassword.iDataLen   = (int)strlen(oLoginParams.sPassword.pData);

  if (!pEngine -> login(&oLoginParams, &iCode)) {
    cout << "REngine::login() error : " << iCode << endl;

    delete pEngine;
    delete pCallbacks;
    delete pAdmCallbacks;

    return (BAD);
  }

  ReplayBarParams replayParams;

  replayParams.sExchange.pData    = argv[3];
  replayParams.sExchange.iDataLen = (int)strlen(replayParams.sExchange.pData);
  replayParams.sTicker.pData      = argv[4];
  replayParams.sTicker.iDataLen   = (int)strlen(replayParams.sTicker.pData);

  replayParams.iType              = BAR_TYPE_MINUTE;
  replayParams.iSpecifiedMinutes  = atoi(argv[5]);

  replayParams.bCustomSession = false;

  replayParams.iStartSsboe = atol(argv[6]);
  replayParams.iStartUsecs = 0;

  replayParams.iEndSsboe   = atol(argv[7]);
  replayParams.iEndUsecs   = 0;

  if (!pEngine -> replayBars(&replayParams, &iCode)) {
    printf("REngine::replayBars() error : %d\n", iCode);
  }

  sleep(2);

  delete pEngine;
  delete pCallbacks;
  delete pAdmCallbacks;

  return (GOOD);
}
