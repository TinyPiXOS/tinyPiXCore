#ifndef GUMBO_INTERFACE
#define GUMBO_INTERFACE

#include <stdlib.h>
#include <string>
#include <unordered_set>

#include "gumbo.h"
#include "gumbo_edit.h"

#include <string>
#include <list>
#include <map>
#include <set>

namespace future {

struct GumboWellFormedError {
	int line;
	int column;
	std::string message;
};

class GumboInterface {
public:

	GumboInterface(const std::string &source, const std::string &version);
	GumboInterface(const std::string &source, const std::string &version,
			const std::map<std::string, std::string> &source_updates);
	~GumboInterface();

	void parse();
	std::string repair();
	std::string getxhtml();
	std::string prettyprint(std::string indent_chars = "  ");

	// returns list tags that match manifest properties
	std::list<std::string> get_all_properties();

	// returns "html" node
	GumboNode * get_root_node();

	// routines for working with gumbo paths
	GumboNode* get_node_from_path(std::list<unsigned int> & apath);
	std::list<unsigned int> get_path_to_node(GumboNode* node);

	// routines for working with qwebpaths
	GumboNode* get_node_from_qwebpath(std::string webpath);
	std::string get_qwebpath_to_node(GumboNode* node);

	// routines for updating while serializing (see SourceUpdates and AnchorUpdates
	std::string perform_source_updates(
			const std::string & my_current_book_relpath);
	std::string perform_style_updates(
			const std::string & my_current_book_relpath);
	std::string perform_link_updates(const std::string & newlinks);
	std::string get_body_contents();
	std::string perform_body_updates(const std::string & new_body);

	// routines for working with nodes with specific attributes
	std::list<GumboNode*> get_all_nodes_with_attribute(
			const std::string & attname);
	std::list<std::string> get_all_values_for_attribute(
			const std::string & attname);
	std::map<std::string, std::string> get_attributes_of_node(GumboNode* node);

	// routines for working with nodes with specific tags
	std::list<GumboNode*> get_all_nodes_with_tag(GumboTag tag);
	std::list<GumboNode*> get_all_nodes_with_tags(
			const std::list<GumboTag> & tags);

	// utility routines
	std::string get_tag_name(GumboNode *node);
	std::string get_local_text_of_node(GumboNode* node);
	std::string get_text_of_node(GumboNode* node);

	// routine to check if well-formed
	std::list<GumboWellFormedError> error_check();

	// routines to work with node and it children only
	std::list<GumboNode*> get_nodes_with_attribute(GumboNode* node,
			const char * att_name);

	std::list<GumboNode*> get_nodes_with_tags(GumboNode* node,
			const std::list<GumboTag> & tags);

	std::list<GumboNode*> get_all_nodes(GumboNode* node);

private:

	enum UpdateTypes {
		NoUpdates = 0,
		SourceUpdates = 1 << 0,
		LinkUpdates = 1 << 1,
		BodyUpdates = 1 << 2,
		StyleUpdates = 1 << 3
	};

	std::list<std::string> get_properties(GumboNode* node);

	std::list<std::string> get_values_for_attr(GumboNode* node,
			const char* attr_name);

	std::string serialize(GumboNode* node,
			enum UpdateTypes doupdates = NoUpdates);

	std::string serialize_contents(GumboNode* node,
			enum UpdateTypes doupdates = NoUpdates);

	std::string prettyprint(GumboNode* node, int lvl,
			const std::string indent_chars);

	std::string prettyprint_contents(GumboNode* node, int lvl,
			const std::string indent_chars);

	std::string build_doctype(GumboNode *node);

	std::string get_attribute_name(GumboAttribute * at);

	std::string build_attributes(GumboAttribute * at, bool no_entities,
			bool run_src_updates = false, bool run_style_updates = false);

	std::string update_attribute_value(const std::string &href);

	std::string update_style_urls(const std::string& source);

	std::string substitute_xml_entities_into_text(const std::string &text);

	std::string substitute_xml_entities_into_attributes(char quote,
			const std::string &text);

	bool in_set(std::set<std::string> &s, std::string &key);

	void rtrim(std::string &s);

	void ltrim(std::string &s);

	void ltrimnewlines(std::string &s);

	void newlinetrim(std::string &s);

	void condense_whitespace(std::string &s);

	void replace_all(std::string &s, const char * s1, const char * s2);

	// Hopefully now unneeded
	// std::string fix_self_closing_tags(const std::string & source);

	std::string m_source;
	GumboOutput* m_output;
	std::string m_utf8src;
	const std::map<std::string, std::string> & m_sourceupdates;
	std::string m_newcsslinks;
	std::string m_currentdir;
	std::string m_newbody;bool m_hasnbsp;
	std::string m_version;

};
}
#endif
