// Copyright 2016 Scott Bevin, All Rights Reserved

#pragma once

#include "AX/Utils/AXParent.h"
#include "AXEditorImGuiWindow.h"

class AXContentBrowserImGuiWindow : public AXParent< AXEditorImGuiWindow, AXEditorImGuiWindow >
{
private:
	struct ContentFoldersTreeNode
	{
		/**
		* A directory object containing details about this directory
		*/
		AXDirectory mDirectory;

		/**
		 * This items name
		 */
		AXString mName;

		/**
		 * Is the node open int he tree view
		 */
		bool mNodeOpen = true;

		/**
		 * The parent of this node
		 */
		ContentFoldersTreeNode* mParent = nullptr;

		/**
		* Child items of this node
		*/
		std::vector< ContentFoldersTreeNode > mChilden;
	};

public:
	/**
	* Constructor
	*/
	AXContentBrowserImGuiWindow();

	/**
	* Destructor
	*/
	~AXContentBrowserImGuiWindow();
	
protected:

	/**
	* Override to render the contents of this editor window
	*/
	virtual void RenderContents( ) override;

private:

	/**
	* Renders the folder tree part of the content browser
	*/
	void RenderFolderTree_Recursive( ContentFoldersTreeNode& node, ContentFoldersTreeNode*& currentlySelectedNode, bool& currentlySelectedNodeValidated );

	/**
	 * Renders the folder tree part of the content browser
	 */
	void RenderFolderTree( );

	/**
	 * Renders the right hand panel showing folder contents
	 */
	void RenderFolderContentsPanel( ContentFoldersTreeNode& folder );

	/**
	 * Renders the folders contents in columns mode
	 */
	void RenderFolderContensPanelAsColumns( ContentFoldersTreeNode& folder );

	/**
	* Looks through the content directory and pulls out all the info we need
	*/
	void AnalyseContentDirectory_Recursive( ContentFoldersTreeNode& node, const AXString& path );

	/**
	 * Looks through the content directory and pulls out all the info we need
	 */
	void AnalyseContentDirectory( );

private:
	/**
	* A node containing the root content directory
	*/
	ContentFoldersTreeNode mRootContentDirectoryNode;

	/**
	 * Pointer to the current directory node we should render information for
	 */
	ContentFoldersTreeNode* mContentDirectoryNodeToDisplay;
};