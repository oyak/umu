#include "idatatransfer.h"

bool cIDataTransfer::updateStatistics(const int connection_id)
{
    if (!isConnectionExist(connection_id)) {
        return false;
    }

    cConnection* p_connection = &_connections_pull[connection_id];

    unsigned long curr_time = 0;

#if (defined(DEFCORE_OS_WIN)) && (!defined(DEFCORE_CC_GNU))
    curr_time = GetTickCount();
#else
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        curr_time = ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
    }
#endif

    const unsigned long update_interval = 400;

    if ((curr_time - p_connection->_prev_update_time) > update_interval) {
        double time_left_sec = double(curr_time - p_connection->_prev_update_time) / 1000.0;

        p_connection->_download_speed = (p_connection->_bytes_downloaded ? (double(p_connection->_bytes_downloaded) / time_left_sec) : 0);

        p_connection->_upload_speed = (p_connection->_bytes_uploaded ? (double(p_connection->_bytes_uploaded) / time_left_sec) : 0);

        p_connection->_bytes_downloaded = p_connection->_bytes_uploaded = 0;
        p_connection->_prev_update_time = curr_time;
    }

    return true;
}


cIDataTransfer::~cIDataTransfer()
{
    for (unsigned i = 0; i < _connections_pull.size(); ++i) {
        closeConnection(i);

        // _socket_server и _socket_host могут быть указателями на один и тот же
        // сокет, в таком случае delete нужно вызвать лиш раз, в противном
        // случае повторный вызоа приведет к UB

        if (_connections_pull[i]._socket_server != _connections_pull[i]._socket_host) {
            delete _connections_pull[i]._socket_host;
            delete _connections_pull[i]._socket_server;
        }
        else {
            delete _connections_pull[i]._socket_host;
        }
        _connections_pull[i]._socket_host = 0;
        _connections_pull[i]._socket_server = 0;

        delete _connections_pull[i]._parameters;
        _connections_pull[i]._parameters = 0;

        delete _connections_pull[i]._recv_buffer;
        _connections_pull[i]._recv_buffer = 0;
    }
}

bool cIDataTransfer::isConnectionExist(int connection_id) const
{
    return ((connection_id >= 0) && (static_cast<unsigned int>(connection_id) < _connections_pull.size()));
}

bool cIDataTransfer::isConnectionOpen(const int connection_id) const
{
    if (isConnectionExist(connection_id)) {
        if (_connections_pull[connection_id].checkConnection()) {
            return true;
        }
    }

    return false;
}

bool cIDataTransfer::closeConnection(const int connection_id)
{
    if (!isConnectionExist(connection_id)) {
        return false;
    }

    if (_connections_pull[connection_id]._socket_host != 0) {
        _connections_pull[connection_id]._socket_host->disconnect();
    }

    if (_connections_pull[connection_id]._socket_server != 0) {
        _connections_pull[connection_id]._socket_server->disconnect();
    }

    SLEEP(10);  // TODO: why 10?

    return true;
}

void cIDataTransfer::closeAllConnections()
{
    for (unsigned i = 0; i < _connections_pull.size(); ++i) {
        closeConnection(i);
    }
}

int cIDataTransfer::getUploadSpeed(const int connection_id) const
{
    if (!isConnectionExist(connection_id)) {
        return -1;
    }

    return _connections_pull[connection_id]._upload_speed;
}

int cIDataTransfer::getDownloadSpeed(const int connection_id) const
{
    if (!isConnectionExist(connection_id)) {
        return -1;
    }

    return _connections_pull[connection_id]._download_speed;
}

void cIDataTransfer::print_hex_memory(const void *mem, int size) {
    int i;
    const unsigned char* p = reinterpret_cast<const unsigned char*>(mem);
    for (i=0; i<size; i++) {
    fprintf( stderr,"%02x ", p[i]);
    }
    fprintf( stderr,"\n");
}

bool cIDataTransfer::write(const int connection_id, const unsigned char* const data_ptr, const unsigned int data_count)
{
    if (!isConnectionOpen(connection_id)) {
        return false;
    }
    bool ret_val = false;
    const int num_of_try = 3;

    for (int try_i = 0; try_i < num_of_try; ++try_i) {

        ret_val = _connections_pull[connection_id]._socket_host->sendData(data_ptr, data_count);

        if (ret_val == true) {
            break;
        }
        if (_connections_pull[connection_id]._socket_host->getSocket() < 0) {
            openConnection(connection_id);
        }
    }

    // Update transfer statistics
    if (ret_val) {
        _connections_pull[connection_id]._bytes_uploaded += data_count;
        updateStatistics(connection_id);
    }

    return ret_val;
}

// +
int cIDataTransfer::read(const int connection_id, unsigned char* const data_ptr, const unsigned int data_count)
{
    if (!isConnectionOpen(connection_id)) {
        return -1;
    }

    int ret_val = _connections_pull[connection_id]._socket_server->reciveData(data_ptr, data_count);

    // Update transfer statistics
    if (ret_val > 0) {
        _connections_pull[connection_id]._bytes_downloaded += data_count;
        updateStatistics(connection_id);
    }

    return ret_val;
}
