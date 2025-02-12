#include "network_audio_sink.h"
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")


NetworkAudioSink::NetworkAudioSink(
	const std::string& address = "127.0.0.1",
	USHORT port = 12345) 
	: sock(INVALID_SOCKET)
	, serverPort(port)
	, serverAddress(address) {
	
	// configure socket
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		throw std::runtime_error("WSAStartup failed");
	}
}

NetworkAudioSink::~NetworkAudioSink() {
	// configuration cleanup
	WSACleanup();
}

void NetworkAudioSink::Start() {
	// establish socket connection
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		throw std::runtime_error("Failed to configure TCP socket");
	}

	sockaddr_in serverAddr = {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverPort);
	inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr);

	if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		throw std::runtime_error("Failed to connect TCP socket");
	}
}

void NetworkAudioSink::Stop() {
	// close socket connection
	if (sock != INVALID_SOCKET) {
		closesocket(sock);
	}
}

void NetworkAudioSink::ConsumeAudioData(const AudioData& data) {
	// if socket established, AudioData.data over network
	uint32_t dataSize = static_cast<uint32_t>(data.data.size());
	if (send(sock, reinterpret_cast<const char*>(&dataSize), sizeof(dataSize), 0) != sizeof(dataSize)) {
		throw std::runtime_error("Failed to send data size over TCP socket");
	}

	size_t bytesSent = 0;
	while (bytesSent < data.data.size()) {
		int sent = send(sock, (const char*)(data.data.data()), dataSize - bytesSent, 0);
		if (sent == SOCKET_ERROR) {
			throw std::runtime_error("Failed to send data over TCP socket");
		}
		bytesSent += sent;
	}
}
