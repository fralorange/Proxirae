namespace Proxirae.Application.Messenger
{
    public enum MessageType : ushort
    {
        Cmd_ReloadProxies = 1,
        Cmd_ReloadRules,
        Cmd_ReloadPreferences,
        Cmd_DisconnectFlow,
        Cmd_EndFlowProcess,
        Cmd_CheckProxy,

        Evt_FlowReceive = 1000,
        Evt_LogReceive,
        Evt_RouteReceive,
        Evt_ReceiveTestProgress
    }
}
