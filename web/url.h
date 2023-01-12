#pragma once

#include <string>
#include <vector>

constexpr int PORT_NULL = -1;
constexpr int FTP_DEFAULT_PORT = 21;
constexpr int FILE_DEFAULT_PORT = PORT_NULL;
constexpr int HTTP_DEFAULT_PORT = 80;
constexpr int HTTPS_DEFAULT_PORT = 443;

class Url
{
public:
	Url() = default;
	Url(const std::string &);
	Url(const std::string &, Url *);

	static int default_port(const std::string &);

	std::string scheme() const { return m_scheme; }
	std::string authority() const { return m_authority; }
	std::string drive() const { return m_drive; }
	std::string path_root() const { return m_path_root; }
	std::string dir() const { return m_dir; }
	std::string file() const { return m_file; }
	std::string query() const { return m_query; }
	std::string fragment() const { return m_fragment; }
	std::string host() const { return m_host; }
	std::vector<std::string> path() const { return m_path; }
	int port() const;

	inline void set_scheme(const std::string &scheme) { m_scheme = scheme; }
	inline void set_authority(const std::string &authority) { m_authority = authority; }
	inline void set_drive(const std::string &drive) { m_drive = drive; }
	inline void set_path_root(const std::string &path_root) { m_path_root = path_root; }
	inline void set_dir(const std::string &dir) { m_dir = dir; }
	inline void set_file(const std::string &file) { m_file = file; }
	inline void set_query(const std::string &query) { m_query = query; }
	inline void set_fragment(const std::string &fragment) { m_fragment = fragment; }
	inline void set_host(const std::string &host) { m_host = host; }
	inline void set_path(const std::vector<std::string> &path) { m_path = path; }
	inline void append_path(const std::string &frag) { m_path.push_back(frag); }
	inline void clear_path() { m_path.clear(); }
	inline void set_port(int port) { m_port = port; }

	bool is_special() const;
	bool has_opaque_path() const { return false; }
	void shorten_path();
	std::string serialize_path() const;
	std::string to_string() const;

private:
	std::string m_scheme;
	std::string m_authority;
	std::string m_drive;
	std::string m_path_root;
	std::string m_dir;
	std::string m_file;
	std::string m_query;
	std::string m_fragment;
	std::string m_host;
	int m_port = PORT_NULL;
	std::vector<std::string> m_path;
};
