#include <gtk/gtk.h>
#include <gdata/gdata.h>
#include "gdata/gdata-service.h"
#include "gdata/gdata-client-login-authorizer.h"
#include <gdata/gdata-authorizer.h>
#include <gdata/services/picasaweb/gdata-picasaweb-service.h>
#include <gdata/gdata-goa-authorizer.h>
#include <gio/gio.h>

int main()
{
	GoaObject *goa_object;	
	GDataPicasaWebService *service;
	GDataPicasaWebAlbum *album, *inserted_album;
	GDataGoaAuthorizer *authorizer;
	GError *error;
	GoaClient *client;
	GList *accounts_list, *l;

	error = NULL;
	/*I am using sync version just to avoid some complexity and keeping it simple.*/
	client = goa_client_new_sync (NULL, &error);

	/*FYI, I have only experimented this with just one google account so account_lists assume 1 account only.*/
	accounts_list = goa_client_get_accounts (client);
	printf ("Accounts : %d\n", g_list_length (accounts_list));

	goa_object = accounts_list->data;
	authorizer = gdata_goa_authorizer_new (goa_object);	
	service = gdata_picasaweb_service_new (GDATA_AUTHORIZER (authorizer));

	/* Create a GDataPicasaWebAlbum entry for the new album, setting some information about it */
	album = gdata_picasaweb_album_new (NULL);
	gdata_entry_set_title (GDATA_ENTRY (album), "Test Album");
	gdata_entry_set_summary (GDATA_ENTRY (album), "This is just a testing album");
	gdata_picasaweb_album_set_location (album, "Ahmedabad");

	/* Insert the new album on the server. Note that this is a blocking operation. */
	inserted_album = gdata_picasaweb_service_insert_album (service, album, NULL, NULL);

	g_object_unref (album);
	g_object_unref (inserted_album);
	g_object_unref (service);
	g_object_unref (authorizer);
	printf("Album Created!");
}
