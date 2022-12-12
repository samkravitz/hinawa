#pragma once

#include <string>

class Url
{
public:
	Url(const std::string &);

	std::string scheme() const { return m_scheme; }
	std::string authority() const { return m_authority; }
	std::string drive() const { return m_drive; }
	std::string path_root() const { return m_path_root; }
	std::string dir() const { return m_dir; }
	std::string file() const { return m_file; }
	std::string query() const { return m_query; }
	std::string fragment() const { return m_fragment; }

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
	std::string m_url_string;
};
