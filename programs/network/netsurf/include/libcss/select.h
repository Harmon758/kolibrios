/*
 * This file is part of LibCSS
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef libcss_select_h_
#define libcss_select_h_

#ifdef __cplusplus
extern "C"
{
#endif

#include <libcss/errors.h>
#include <libcss/functypes.h>
#include <libcss/hint.h>
#include <libcss/types.h>
#include <libcss/computed.h>

typedef enum css_pseudo_element {
	CSS_PSEUDO_ELEMENT_NONE         = 0,
	CSS_PSEUDO_ELEMENT_FIRST_LINE   = 1,
	CSS_PSEUDO_ELEMENT_FIRST_LETTER = 2,
	CSS_PSEUDO_ELEMENT_BEFORE       = 3,
	CSS_PSEUDO_ELEMENT_AFTER        = 4,

	CSS_PSEUDO_ELEMENT_COUNT	= 5	/**< Number of pseudo elements */
} css_pseudo_element;

/**
 * Style selection result set
 */
typedef struct css_select_results {
	css_allocator_fn alloc;
	void *pw;

	/**
	 * Array of pointers to computed styles, 
	 * indexed by css_pseudo_element. If there
	 * was no styling for a given pseudo element, 
	 * then no computed style will be created and
	 * the corresponding pointer will be set to NULL
	 */
	css_computed_style *styles[CSS_PSEUDO_ELEMENT_COUNT];
} css_select_results;

typedef enum css_select_handler_version {
	CSS_SELECT_HANDLER_VERSION_1 = 1
} css_select_handler_version;

typedef struct css_select_handler {
	/** ABI version of this structure */
	uint32_t handler_version;

	css_error (*node_name)(void *pw, void *node,
			css_qname *qname);
	css_error (*node_classes)(void *pw, void *node,
			lwc_string ***classes,
			uint32_t *n_classes);
	css_error (*node_id)(void *pw, void *node,
			lwc_string **id);

	css_error (*named_ancestor_node)(void *pw, void *node,
			const css_qname *qname, void **ancestor);
	css_error (*named_parent_node)(void *pw, void *node,
			const css_qname *qname, void **parent);
	css_error (*named_sibling_node)(void *pw, void *node,
			const css_qname *qname, void **sibling);
	css_error (*named_generic_sibling_node)(void *pw, void *node,
			const css_qname *qname, void **sibling);

	css_error (*parent_node)(void *pw, void *node, void **parent);
	css_error (*sibling_node)(void *pw, void *node, void **sibling);

	css_error (*node_has_name)(void *pw, void *node,
			const css_qname *qname, bool *match);
	css_error (*node_has_class)(void *pw, void *node,
			lwc_string *name, bool *match);
	css_error (*node_has_id)(void *pw, void *node,
			lwc_string *name, bool *match);
	css_error (*node_has_attribute)(void *pw, void *node,
			const css_qname *qname, bool *match);
	css_error (*node_has_attribute_equal)(void *pw, void *node,
			const css_qname *qname, lwc_string *value,
			bool *match);
	css_error (*node_has_attribute_dashmatch)(void *pw, void *node,
			const css_qname *qname, lwc_string *value,
			bool *match);
	css_error (*node_has_attribute_includes)(void *pw, void *node,
			const css_qname *qname, lwc_string *value,
			bool *match);
	css_error (*node_has_attribute_prefix)(void *pw, void *node,
			const css_qname *qname, lwc_string *value,
			bool *match);
	css_error (*node_has_attribute_suffix)(void *pw, void *node,
			const css_qname *qname, lwc_string *value,
			bool *match);
	css_error (*node_has_attribute_substring)(void *pw, void *node,
			const css_qname *qname, lwc_string *value,
			bool *match);

	css_error (*node_is_root)(void *pw, void *node, bool *match);
	css_error (*node_count_siblings)(void *pw, void *node,
			bool same_name, bool after, int32_t *count);
	css_error (*node_is_empty)(void *pw, void *node, bool *match);

	css_error (*node_is_link)(void *pw, void *node, bool *match);
	css_error (*node_is_visited)(void *pw, void *node, bool *match);
	css_error (*node_is_hover)(void *pw, void *node, bool *match);
	css_error (*node_is_active)(void *pw, void *node, bool *match);
	css_error (*node_is_focus)(void *pw, void *node, bool *match);

	css_error (*node_is_enabled)(void *pw, void *node, bool *match);
	css_error (*node_is_disabled)(void *pw, void *node, bool *match);
	css_error (*node_is_checked)(void *pw, void *node, bool *match);

	css_error (*node_is_target)(void *pw, void *node, bool *match);
	css_error (*node_is_lang)(void *pw, void *node,
			lwc_string *lang, bool *match);

	css_error (*node_presentational_hint)(void *pw, void *node, 
			uint32_t property, css_hint *hint);

	css_error (*ua_default_for_property)(void *pw, uint32_t property,
			css_hint *hint);

	css_error (*compute_font_size)(void *pw, const css_hint *parent,
			css_hint *size);
} css_select_handler;

/**
 * Font face selection result set
 */
typedef struct css_select_font_faces_results {
	css_allocator_fn alloc;
	void *pw;
	
	/**
	 * Array of pointers to computed font faces. 
	 */
	css_font_face **font_faces;
	uint32_t n_font_faces;
} css_select_font_faces_results;

css_error css_select_ctx_create(css_allocator_fn alloc, void *pw,
		css_select_ctx **result);
css_error css_select_ctx_destroy(css_select_ctx *ctx);

css_error css_select_ctx_append_sheet(css_select_ctx *ctx, 
		const css_stylesheet *sheet, 
		css_origin origin, uint64_t media);
css_error css_select_ctx_insert_sheet(css_select_ctx *ctx,
		const css_stylesheet *sheet, uint32_t index,
		css_origin origin, uint64_t media);
css_error css_select_ctx_remove_sheet(css_select_ctx *ctx,
		const css_stylesheet *sheet);

css_error css_select_ctx_count_sheets(css_select_ctx *ctx, uint32_t *count);
css_error css_select_ctx_get_sheet(css_select_ctx *ctx, uint32_t index,
		const css_stylesheet **sheet);

css_error css_select_style(css_select_ctx *ctx, void *node,
		uint64_t media, const css_stylesheet *inline_style,
		css_select_handler *handler, void *pw,
		css_select_results **result);
css_error css_select_results_destroy(css_select_results *results);    

css_error css_select_font_faces(css_select_ctx *ctx,
		uint64_t media, lwc_string *font_family,
		css_select_font_faces_results **result);
css_error css_select_font_faces_results_destroy(
		css_select_font_faces_results *results);	

#ifdef __cplusplus
}
#endif

#endif
