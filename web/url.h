#pragma once

#include <string>

class Url
{
public:
	Url() = default;
	Url(const std::string &);

	std::string scheme() const { return m_scheme; }
	std::string authority() const { return m_authority; }
	std::string drive() const { return m_drive; }
	std::string path_root() const { return m_path_root; }
	std::string dir() const { return m_dir; }
	std::string file() const { return m_file; }
	std::string query() const { return m_query; }
	std::string fragment() const { return m_fragment; }

	inline void set_scheme(const std::string &scheme) { m_scheme = scheme; }
	inline void set_authority(const std::string &authority) { m_authority = authority; }
	inline void set_drive(const std::string &drive) { m_drive = drive; }
	inline void set_path_root(const std::string &path_root) { m_path_root = path_root; }
	inline void set_dir(const std::string &dir) { m_dir = dir; }
	inline void set_file(const std::string &file) { m_file = file; }
	inline void set_query(const std::string &query) { m_query = query; }
	inline void set_fragment(const std::string &fragment) { m_fragment = fragment; }

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
};
