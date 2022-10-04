#pragma once

#include <wchar.h>
#include <windows.h>

#include <string>
#include <functional>
#include <vector>
#include <chrono>

//Define HID_API_STATIC in project settings if you want static link

#ifndef HID_API_STATIC	

//DLL link
#ifdef HID_API_BUILD
      #define HID_API_EXPORT __declspec(dllexport)
      #define HID_API_CALL
#else
      #define HID_API_EXPORT __declspec(dllimport)
      #define HID_API_CALL
#endif

#else

//Static link
#define HID_API_EXPORT
#define HID_API_CALL

#ifndef HID_API_BUILD
#pragma comment(lib, "setupapi.lib")
#endif

#endif

#define HID_API_EXPORT_CALL HID_API_EXPORT HID_API_CALL

class HID_API_EXPORT HIDDevice
{
private:
	HANDLE _device_handle;
	BOOL _blocking;
	USHORT _output_report_length;
	DWORD _input_report_length;
	DWORD _feature_report_length;
	std::wstring _last_error_str;
	DWORD _last_error_num;
	BOOL _read_pending;
	char* _read_buf;
	OVERLAPPED _ol;
	std::string _path;
	std::wstring _serial;
	uint16_t _vid;
	uint16_t _pid;
	std::wstring _man;
	std::wstring _prod;
	uint16_t _relN;
	int _ifaceN;

	HIDDevice(HIDDevice&) = delete;
	HIDDevice& operator=(HIDDevice&) = delete;

	bool _openDevice();
	bool _openHandle(bool enumerate);
	void _makeError();
	void _freeDevice();

	friend void* _hid_enumerate(uint16_t vendor_id, uint16_t product_id);

public:
	HIDDevice();
	~HIDDevice();

	struct DevInfo
	{
		const uint16_t vendorId;
		const uint16_t productId;
		const std::string aserial;
		const std::wstring wserial;
		const std::string path;
		const std::string aprod;
		const std::wstring wprod;
		const std::string aman;
		const std::wstring wman;
	};

	static void EnumDevices(uint16_t vendor_id, uint16_t product_id, const std::function<void(DevInfo)>& enumerator);

	uint16_t GetVendorID() const;
	uint16_t GetProductID() const;
	std::string GetSerialA() const;
	std::wstring GetSerialW() const;
	std::string GetPath() const;
	std::string GetManufacturerA() const;
	std::wstring GetManufacturerW() const;
	std::string GetProductStrA() const;
	std::wstring GetProductStrW() const;
	uint16_t GetReleaseNum() const;
	int GetInterfaceNum() const;
	std::string GetIndexedStrA(int index) const;
	std::wstring GetIndexedStrW(int index) const;
	DWORD GetLastErrorNum() const;
	std::string GetLastErrorStrA() const;
	std::wstring GetLastErrorStrW() const;
	USHORT GetOutputReportLength() const;
	DWORD GetInputReportLength() const;
	DWORD GetFeatureReportLength() const;
	bool IsOpen() const;

	bool Open(const std::string& path, bool subString = false);
	bool Open(uint16_t vendorId, uint16_t productId, const std::wstring serial = L"");
	bool Open(uint16_t vendorId, uint16_t productId, const std::string serial = "");
	void Close();

	int Read(std::vector<byte>& buffer);
	int ReadTimeout(std::vector<byte>& buffer, std::chrono::milliseconds timeMs);
	int Write(const std::vector<byte>& data);
	void SetNonblocking(bool nonblock);
	int SendFeatureReport(const std::vector<byte>& data);
	int GetFeatureReport(std::vector<byte>& buffer);
};