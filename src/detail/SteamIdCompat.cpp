// Pinned-SteamSDK compatibility symbols.
//
// steamclientpublic.h in thirdparty/SteamSDK declares CSteamID's out-of-line
// methods (constructor from text, SetFromString, Render, BValidExternalSteamID,
// Render(uint64)) but the pinned header-only SDK does not define them. AbcCore
// does not link steam_api.lib, so these definitions live here as an explicit
// compatibility TU instead of being mixed into the abc::SteamID parser.
//
// If the pinned SDK ever provides the definitions (or the project links
// steam_api.lib), delete this file rather than keeping duplicate symbols.

#include <abc/SteamId.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>

// Valve parse code uses the tier0-style `uint` typedef.
using uint = unsigned int;

#define V_snprintf sprintf_s

//-----------------------------------------------------------------------------
// Purpose: Constructor
// Input  : pchSteamID -		text representation of a Steam ID
//-----------------------------------------------------------------------------
CSteamID::CSteamID(const char *pchSteamID,
                   EUniverse eDefaultUniverse /* = k_EUniverseInvalid */) {
  SetFromString(pchSteamID, eDefaultUniverse);
}

//-----------------------------------------------------------------------------
// Purpose: Initializes a steam ID from a string
// Input  : pchSteamID -		text representation of a Steam ID
//-----------------------------------------------------------------------------
void CSteamID::SetFromString(const char *pchSteamID,
                             EUniverse eDefaultUniverse) {
  if (!pchSteamID) {
    Clear();
    return;
  }
  if (strncmp(pchSteamID, "STEAM_", 6) == 0) {
    *this = abc::ParseSteamID(pchSteamID);
    return;
  }
  uint nAccountID = 0;
  uint nInstance = 1;
  EUniverse eUniverse = eDefaultUniverse;
  // Since we are using sscanf with %d to read this value
  // we need eUniverse to be the size of an int.
  EAccountType eAccountType = k_EAccountTypeIndividual;
  if (*pchSteamID == '[')
    pchSteamID++;

  if (*pchSteamID == 'A') {
    // This is test only
    pchSteamID++; // skip the A
    eAccountType = k_EAccountTypeAnonGameServer;
    if (*pchSteamID == '-' || *pchSteamID == ':')
      pchSteamID++; // skip the optional - or :

    if (strchr(pchSteamID, '('))
      sscanf(strchr(pchSteamID, '('), "(%u)", &nInstance);
    const char *pchColon = strchr(pchSteamID, ':');
    if (pchColon && *pchColon != 0 && strchr(pchColon + 1, ':')) {
      sscanf(pchSteamID, "%u:%u:%u", (uint *)&eUniverse, &nAccountID,
             &nInstance);
    } else if (pchColon) {
      sscanf(pchSteamID, "%u:%u", (uint *)&eUniverse, &nAccountID);
    } else {
      sscanf(pchSteamID, "%u", &nAccountID);
    }

    if (nAccountID == 0) {
      // i dont care what number you entered
      CreateBlankAnonLogon(eUniverse);
    } else {
      InstancedSet(nAccountID, nInstance, eUniverse, eAccountType);
    }
    return;
  } else if (*pchSteamID == 'G') {
    pchSteamID++; // skip the G
    eAccountType = k_EAccountTypeGameServer;
    if (*pchSteamID == '-' || *pchSteamID == ':')
      pchSteamID++; // skip the optional - or :
  } else if (*pchSteamID == 'C') {
    pchSteamID++; // skip the C
    eAccountType = k_EAccountTypeContentServer;
    if (*pchSteamID == '-' || *pchSteamID == ':')
      pchSteamID++; // skip the optional - or :
  } else if (*pchSteamID == 'g') {
    pchSteamID++; // skip the g
    eAccountType = k_EAccountTypeClan;
    nInstance = 0;
    if (*pchSteamID == '-' || *pchSteamID == ':')
      pchSteamID++; // skip the optional - or :
  } else if (*pchSteamID == 'c') {
    pchSteamID++; // skip the c
    eAccountType = k_EAccountTypeChat;
    nInstance = k_EChatInstanceFlagClan;
    if (*pchSteamID == '-' || *pchSteamID == ':')
      pchSteamID++; // skip the optional - or :
  } else if (*pchSteamID == 'L') {
    pchSteamID++; // skip the L
    eAccountType = k_EAccountTypeChat;
    nInstance = k_EChatInstanceFlagLobby;
    if (*pchSteamID == '-' || *pchSteamID == ':')
      pchSteamID++; // skip the optional - or :
  } else if (*pchSteamID == 'T') {
    pchSteamID++; // skip the T
    eAccountType = k_EAccountTypeChat;
    nInstance = 0; // Anon chat
    if (*pchSteamID == '-' || *pchSteamID == ':')
      pchSteamID++; // skip the optional - or :
  } else if (*pchSteamID == 'U') {
    pchSteamID++; // skip the U
    eAccountType = k_EAccountTypeIndividual;
    nInstance = 1;
    if (*pchSteamID == '-' || *pchSteamID == ':')
      pchSteamID++; // skip the optional - or :
  } else if (*pchSteamID == 'i') {
    pchSteamID++; // skip the i
    eAccountType = k_EAccountTypeInvalid;
    nInstance = 1;
    if (*pchSteamID == '-' || *pchSteamID == ':')
      pchSteamID++; // skip the optional - or :
  } else {
    // Check for a 64-bit steamID, unadorned.
    uint64 unSteamID64 = _strtoui64(pchSteamID, nullptr, 10);
    if (unSteamID64 > 0xffffffffU) {
      SetFromUint64(unSteamID64);
      return;
    }
  }

  if (strchr(pchSteamID, ':')) {
    if (*pchSteamID == '[')
      pchSteamID++; // skip the optional [
    sscanf(pchSteamID, "%u:%u", (uint *)&eUniverse, &nAccountID);
    if (eUniverse == k_EUniverseInvalid)
      eUniverse = eDefaultUniverse;
  } else {
    sscanf(pchSteamID, "%u", &nAccountID);
  }

  InstancedSet(nAccountID, nInstance, eUniverse, eAccountType);
}

//-----------------------------------------------------------------------------
// Purpose: Renders the steam ID to a buffer.  NOTE: for convenience of calling
//			code, this code returns a pointer to a thread-local ring
//buffer (valid until four more renders on this thread).
// Output:  buffer with rendered Steam ID
//-----------------------------------------------------------------------------
const char *CSteamID::Render() const {
  // longest length of returned string is k_cBufLen
  //	[A:%u:%u:%u]
  //	 %u == 10 * 3 + 6 == 36, plus terminator == 37
  const int k_cBufLen = 37;

  const int k_cBufs = 4; // # of static bufs to use (so people can compose
                         // output with multiple calls to Render() )
  static thread_local char rgchBuf[k_cBufs][k_cBufLen];
  static thread_local int nBuf = 0;
  char *pchBuf = rgchBuf[nBuf]; // get pointer to current static buf
  nBuf++;                       // use next buffer for next call to this method
  nBuf %= k_cBufs;

  if (k_EAccountTypeAnonGameServer == m_steamid.m_comp.m_EAccountType) {
    V_snprintf(pchBuf, k_cBufLen, "[A:%u:%u:%u]", m_steamid.m_comp.m_EUniverse,
               m_steamid.m_comp.m_unAccountID,
               m_steamid.m_comp.m_unAccountInstance);
  } else if (k_EAccountTypeGameServer == m_steamid.m_comp.m_EAccountType) {

    V_snprintf(pchBuf, k_cBufLen, "[G:%u:%u]", m_steamid.m_comp.m_EUniverse,
               m_steamid.m_comp.m_unAccountID);
  } else if (k_EAccountTypeMultiseat == m_steamid.m_comp.m_EAccountType) {
    V_snprintf(pchBuf, k_cBufLen, "[M:%u:%u:%u]", m_steamid.m_comp.m_EUniverse,
               m_steamid.m_comp.m_unAccountID,
               m_steamid.m_comp.m_unAccountInstance);
  } else if (k_EAccountTypePending == m_steamid.m_comp.m_EAccountType) {
    V_snprintf(pchBuf, k_cBufLen, "[P:%u:%u]", m_steamid.m_comp.m_EUniverse,
               m_steamid.m_comp.m_unAccountID);
  } else if (k_EAccountTypeContentServer == m_steamid.m_comp.m_EAccountType) {
    V_snprintf(pchBuf, k_cBufLen, "[C:%u:%u]", m_steamid.m_comp.m_EUniverse,
               m_steamid.m_comp.m_unAccountID);
  } else if (k_EAccountTypeClan == m_steamid.m_comp.m_EAccountType) {
    // 'g' for "group"
    V_snprintf(pchBuf, k_cBufLen, "[g:%u:%u]", m_steamid.m_comp.m_EUniverse,
               m_steamid.m_comp.m_unAccountID);
  } else if (k_EAccountTypeChat == m_steamid.m_comp.m_EAccountType) {
    if (m_steamid.m_comp.m_unAccountInstance & k_EChatInstanceFlagClan) {
      V_snprintf(pchBuf, k_cBufLen, "[c:%u:%u]", m_steamid.m_comp.m_EUniverse,
                 m_steamid.m_comp.m_unAccountID);
    } else if (m_steamid.m_comp.m_unAccountInstance &
               k_EChatInstanceFlagLobby) {
      V_snprintf(pchBuf, k_cBufLen, "[L:%u:%u]", m_steamid.m_comp.m_EUniverse,
                 m_steamid.m_comp.m_unAccountID);
    } else // Anon chat
    {
      V_snprintf(pchBuf, k_cBufLen, "[T:%u:%u]", m_steamid.m_comp.m_EUniverse,
                 m_steamid.m_comp.m_unAccountID);
    }
  } else if (k_EAccountTypeInvalid == m_steamid.m_comp.m_EAccountType) {
    V_snprintf(pchBuf, k_cBufLen, "[I:%u:%u]", m_steamid.m_comp.m_EUniverse,
               m_steamid.m_comp.m_unAccountID);
  } else if (k_EAccountTypeIndividual == m_steamid.m_comp.m_EAccountType) {
    V_snprintf(pchBuf, k_cBufLen, "[U:%u:%u]", m_steamid.m_comp.m_EUniverse,
               m_steamid.m_comp.m_unAccountID);
  } else if (k_EAccountTypeAnonUser == m_steamid.m_comp.m_EAccountType) {
    V_snprintf(pchBuf, k_cBufLen, "[a:%u:%u]", m_steamid.m_comp.m_EUniverse,
               m_steamid.m_comp.m_unAccountID);
  } else {
    V_snprintf(pchBuf, k_cBufLen, "[i:%u:%u]", m_steamid.m_comp.m_EUniverse,
               m_steamid.m_comp.m_unAccountID);
  }
  return pchBuf;
}

//-----------------------------------------------------------------------------
// Purpose: Renders the passed-in steam ID to a buffer.  NOTE: for convenience
// of calling
//			code, this code returns a pointer to a thread-local ring
//buffer (valid until four more renders on this thread).
// Input:	64-bit representation of Steam ID to render
// Output:  buffer with rendered Steam ID
//-----------------------------------------------------------------------------
const char *CSteamID::Render(uint64 ulSteamID) {
  CSteamID steamID(ulSteamID);
  return steamID.Render();
}

//-----------------------------------------------------------------------------
// Purpose: some steamIDs are for internal use only
// This is really debug code, but we run with asserts on in retail, so ...
//-----------------------------------------------------------------------------
bool CSteamID::BValidExternalSteamID() const {
  if (m_steamid.m_comp.m_EAccountType == k_EAccountTypePending)
    return false;
  if (m_steamid.m_comp.m_EAccountType != k_EAccountTypeAnonGameServer &&
      m_steamid.m_comp.m_EAccountType != k_EAccountTypeContentServer &&
      m_steamid.m_comp.m_EAccountType != k_EAccountTypeAnonUser) {
    if (m_steamid.m_comp.m_unAccountID == 0 &&
        m_steamid.m_comp.m_unAccountInstance == 0)
      return false;
  }
  return true;
}
