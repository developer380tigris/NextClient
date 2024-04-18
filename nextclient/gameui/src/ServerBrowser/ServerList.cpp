#include "ServerList.h"

CServerList::CServerList(IServerRefreshResponse *response_target) :
    response_target_(response_target)
{

}

CServerList::~CServerList()
{
    Clear();
}

void CServerList::RequestFavorites(MatchMakingKeyValuePair_t **ppchFilters, uint32 nFilters)
{
    Clear();

    server_list_request_ = SteamMatchmakingServers()->RequestFavoritesServerList(SteamUtils()->GetAppID(), ppchFilters, nFilters, this);
}

void CServerList::RequestInternet(MatchMakingKeyValuePair_t **ppchFilters, uint32 nFilters)
{
    Clear();

    server_list_request_ = SteamMatchmakingServers()->RequestInternetServerList(SteamUtils()->GetAppID(), ppchFilters, nFilters, this);
}

void CServerList::RequestUnique(MatchMakingKeyValuePair_t **ppchFilters, uint32 nFilters)
{
    Clear();
}

void CServerList::RequestHistory(MatchMakingKeyValuePair_t **ppchFilters, uint32 nFilters)
{
    Clear();

    server_list_request_ = SteamMatchmakingServers()->RequestHistoryServerList(SteamUtils()->GetAppID(), ppchFilters, nFilters, this);
}

void CServerList::RequestFriends(MatchMakingKeyValuePair_t **ppchFilters, uint32 nFilters)
{
    Clear();

    server_list_request_ = SteamMatchmakingServers()->RequestFriendsServerList(SteamUtils()->GetAppID(), ppchFilters, nFilters, this);
}

void CServerList::RequestLan()
{
    Clear();

    server_list_request_ = SteamMatchmakingServers()->RequestLANServerList(SteamUtils()->GetAppID(), this);
}

bool CServerList::IsServerExists(int iServer)
{
    return servers_.count(iServer) > 0;
}

serveritem_t &CServerList::GetServer(int iServer)
{
    return servers_.at(iServer);
}

unsigned int CServerList::ServerCount()
{
    if (server_list_request_ == nullptr)
        return 0;

    return SteamMatchmakingServers()->GetServerCount(server_list_request_);
}

void CServerList::StartRefreshServer(int iServer)
{
    if (server_list_request_ == nullptr)
        return;

    if (servers_.count(iServer) == 0)
        return;

    SteamMatchmakingServers()->RefreshServer(server_list_request_, iServer);
}

void CServerList::StartRefresh()
{
    if (server_list_request_ == nullptr)
        return;

    SteamMatchmakingServers()->RefreshQuery(server_list_request_);
}

void CServerList::StopRefresh(IGameList::CancelQueryReason reason)
{
    if (server_list_request_ == nullptr)
        return;

    SteamMatchmakingServers()->CancelQuery(server_list_request_);
}

void CServerList::Clear()
{
    if (server_list_request_ == nullptr)
        return;

    if (IsRefreshing())
        SteamMatchmakingServers()->CancelQuery(server_list_request_);

    SteamMatchmakingServers()->ReleaseRequest(server_list_request_);

    server_list_request_ = nullptr;
    servers_.clear();
}

bool CServerList::IsRefreshing()
{
    if (server_list_request_ == nullptr)
        return false;

    return SteamMatchmakingServers()->IsRefreshing(server_list_request_);
}

std::unordered_map<int, serveritem_t>::iterator CServerList::begin()
{
    return servers_.begin();
}

std::unordered_map<int, serveritem_t>::iterator CServerList::end()
{
    return servers_.end();
}

void CServerList::ServerResponded(HServerListRequest hRequest, int iServer)
{
    if (server_list_request_ != hRequest)
        return;

    UpdateServerItem(true, iServer);

    response_target_->ServerResponded(servers_.at(iServer));
}

void CServerList::ServerFailedToRespond(HServerListRequest hRequest, int iServer)
{
    if (server_list_request_ != hRequest)
        return;

    UpdateServerItem(false, iServer);

    response_target_->ServerFailedToRespond(servers_.at(iServer));
}

void CServerList::RefreshComplete(HServerListRequest hRequest, EMatchMakingServerResponse response)
{
    if (server_list_request_ != hRequest)
        return;

    response_target_->RefreshComplete();
}

void CServerList::UpdateServerItem(bool successful_response, int iServer)
{
    auto server_details = SteamMatchmakingServers()->GetServerDetails(server_list_request_, iServer);

    if (servers_.count(iServer))
    {
        servers_.at(iServer).gs = *server_details;
        servers_.at(iServer).hadSuccessfulResponse = successful_response;
    }
    else
        servers_.emplace(iServer, serveritem_t(successful_response, iServer, *server_details));
}