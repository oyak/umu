#include "datatransfer_lan.h"

#define OK 0
#define ERR (-(__LINE__))

cDataTransferLan::~cDataTransferLan()
{
    for (unsigned i = 0; i < _connections_pull.size(); ++i) {
        closeConnection(i);

        if (_connections_pull[i]._socket_1 != _connections_pull[i]._socket_2) {
            delete _connections_pull[i]._socket_1;
            delete _connections_pull[i]._socket_2;
        }
        else {
            delete _connections_pull[i]._socket_1;
        }
        _connections_pull[i]._socket_1 = 0;
        _connections_pull[i]._socket_2 = 0;
    }
}

int cDataTransferLan::addConnection(const cISocket::cConnectionParams* const connection_params)
{
    if (connection_params == NULL) {
        return ERR;
    }

    cISocket* s1 = NULL;
    cISocket* s2 = NULL;

    const cLanConnectionParams* lan_connection_params = static_cast<const cLanConnectionParams* const>(connection_params);

    if (lan_connection_params->_socket_1_tcp) {
        s1 = new (std::nothrow) cSocketLanTcp;
    }
    else {
        s1 = new (std::nothrow) cSocketLanUdp;
    }
    if (s1 == NULL) {
        return ERR;
    }

    if (lan_connection_params->_port_out == lan_connection_params->_port_in) {
        s2 = s1;
    }
    else {
        if (lan_connection_params->_socket_2_tcp) {
            s2 = new (std::nothrow) cSocketLanTcp;
        }
        else {
            s2 = new (std::nothrow) cSocketLanUdp;
        }
    }
    if (s2 == NULL) {
        delete s1;
        return ERR;
    }

    const int connection_id = _connections_pull.size();

    _connections_pull.resize(connection_id + 1);


    _connections_pull[connection_id]._socket_1 = s1;
    _connections_pull[connection_id]._socket_2 = s2;

    cLanConnectionParams* param = new (std::nothrow) cLanConnectionParams;

    if (param == NULL) {
        _connections_pull[connection_id]._parameters = 0;
        return ERR;
    }

    cBufferData* p_buff = new (std::nothrow) cBufferData;

    if (p_buff == NULL) {
        delete param;
        return ERR;
    }

    memset(p_buff->_buffer, 0, sizeof(unsigned char) * cBufferData::_buffer_size);
    p_buff->_current_position = 0;
    p_buff->_max_id = 0;
    _connections_pull[connection_id]._recv_buffer = p_buff;

    _connections_pull[connection_id]._parameters = param;

    memcpy(param->_local_ip, lan_connection_params->_local_ip, sizeof(char) * cLanConnectionParams::_ip_size);

    memcpy(param->_remote_ip, lan_connection_params->_remote_ip, sizeof(char) * cLanConnectionParams::_ip_size);

    param->_port_in = lan_connection_params->_port_in;
    param->_port_out = lan_connection_params->_port_out;
    param->_socket_1_tcp = lan_connection_params->_socket_1_tcp;
    param->_socket_2_tcp = lan_connection_params->_socket_2_tcp;
    param->_socket_1_server = lan_connection_params->_socket_1_server;
    param->_socket_2_server = lan_connection_params->_socket_2_server;
    param->_socket_1_transfer_direction = lan_connection_params->_socket_1_transfer_direction;

    return connection_id;
}

// +
int cDataTransferLan::openConnection(const int connection_id)
{
    if (!isConnectionExist(connection_id)) {
        return ERR;
    }

    cISocket* s1 = _connections_pull[connection_id]._socket_1;
    cISocket* s2 = _connections_pull[connection_id]._socket_2;

    cLanConnectionParams* p_param = 0;
    p_param = static_cast<cLanConnectionParams*>(_connections_pull[connection_id]._parameters);
    if (p_param == NULL) {
        return ERR;
    }
    if ((s1 == NULL) || (s2 == NULL)) {
        return ERR;
    }

    bool bool_result = false;
    p_param->_local_ip_address = p_param->_local_ip;
    p_param->_remoute_ip_address = p_param->_remote_ip;
    p_param->_remoute_port_num = p_param->_port_out;
    p_param->_local_port_num = p_param->_port_in;
    if ((p_param->_port_out != p_param->_port_in)) {
        p_param->_server = p_param->_socket_1_server;
        DEFCORE_ASSERT((p_param->_socket_1_transfer_direction == cSocketLan::DirectionToLocal) || (p_param->_socket_1_transfer_direction == cSocketLan::DirectionToRemoute));
        p_param->_transferDirection = p_param->_socket_1_transfer_direction;
        bool_result = s1->connect(p_param);
        if (bool_result) {
            p_param->_server = p_param->_socket_2_server;
            if (p_param->_socket_1_transfer_direction == cSocketLan::DirectionToLocal)
            {
                p_param->_transferDirection = cSocketLan::DirectionToRemoute;
            }
                else
                {
                    p_param->_transferDirection = cSocketLan::DirectionToLocal;
                }
            bool_result = s2->connect(p_param);
        }
    }
    else {
        p_param->_server = p_param->_socket_1_server;
        p_param->_transferDirection = cSocketLan::DirectionBoth;
        bool_result = s1->connect(p_param);
    }

    if (!bool_result) {
        if (s1 != NULL) {
            s1->disconnect();
        }
        if (s1 != s2) {
            s2->disconnect();
        }

        return ERR;
    }
    return OK;
}

bool cDataTransferLan::write(const int connection_id, const unsigned char* const data_ptr, const unsigned int data_count)
{
bool ret_val = false;
const cLanConnectionParams* params = static_cast<const cLanConnectionParams*>(_connections_pull[connection_id]._parameters);

    if (!isConnectionOpen(connection_id)) {
        return false;
    }

    if (params->_socket_1_transfer_direction == cSocketLan::DirectionToLocal)
    {
        ret_val = _connections_pull[connection_id]._socket_2->sendData(data_ptr, data_count);
    }
        else
        {
            ret_val = _connections_pull[connection_id]._socket_1->sendData(data_ptr, data_count);
        }
    // Update transfer statistics
    if (ret_val) {
        _connections_pull[connection_id]._bytes_uploaded += data_count;
        updateStatistics(connection_id);
    }

    return ret_val;
}
//
int cDataTransferLan::read(const int connection_id, unsigned char* data_ptr, const unsigned int data_count)
{
    if (isConnectionExist(connection_id)) {
        if (!(_connections_pull[connection_id].checkConnection())) {
            return ERR;
        }
    }
    else {
        return ERR;
    }

    int ret_val = -1;
    int count = data_count;

    cBufferData* pdata = _connections_pull[connection_id]._recv_buffer;
    int presentDataCount = pdata->_max_id - pdata->_current_position;
    if (count > presentDataCount) {
        const cLanConnectionParams* params = static_cast<const cLanConnectionParams*>(_connections_pull[connection_id]._parameters);
        if (pdata->_current_position) {
            if (presentDataCount) { // смещаем еще несчитанные данные в начало буфера
                memcpy(pdata->_buffer, &(pdata->_buffer[pdata->_current_position]), presentDataCount);
            }
            pdata->_current_position = 0;
            pdata->_max_id = presentDataCount;
        }
        if (params->_socket_1_transfer_direction == cSocketLan::DirectionToRemoute) {
            ret_val = _connections_pull[connection_id]._socket_2->reciveData(&pdata->_buffer[presentDataCount], pdata->_buffer_size - presentDataCount);
        }
            else {
                ret_val = _connections_pull[connection_id]._socket_1->reciveData(&pdata->_buffer[presentDataCount], pdata->_buffer_size - presentDataCount);
            }
        if (ret_val >= 0) {
            pdata->_max_id += ret_val;
            count = std::min(ret_val + presentDataCount, count);
        }
            else {
                SLEEP(1);
                return ret_val;
            }
    }
    memcpy(data_ptr, &(pdata->_buffer[pdata->_current_position]), count);
    // Update transfer statistics
    if (ret_val > 0) {
        _connections_pull[connection_id]._bytes_downloaded += count;
        updateStatistics(connection_id);
    }

    pdata->_current_position += count;
    return count;
}
//
bool cDataTransferLan::closeConnection(const int connection_id)
{
    if (!isConnectionExist(connection_id)) {
        return false;
    }

    if (_connections_pull[connection_id]._socket_1 != 0) {
        _connections_pull[connection_id]._socket_1->disconnect();
    }

    if (_connections_pull[connection_id]._socket_2 != 0) {
        _connections_pull[connection_id]._socket_2->disconnect();
    }

    SLEEP(10);  // TODO: why 10?
    return true;
}
