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
	GDataPicasaWebAlbum *album;
	GDataPicasaWebService *service;
	GDataFeed *album_feed;
	GDataGoaAuthorizer *authorizer;
	GError *error;
	GoaClient *client;
	GList *accounts_list, *albums_list, *l;

	error = NULL;
	/*I am using sync version just to avoid some complexity and keeping it simple.*/
	client = goa_client_new_sync (NULL, &error);
	if (error != NULL)
	{
		printf("Cannot create goa client\n");
		g_error_free (error);
		return -1;
	}
	/*I have only experimented this with just one google account so account_lists assume 1 account only.*/
	accounts_list = goa_client_get_accounts (client);
	printf ("Accounts : %d\n", g_list_length (accounts_list));

	goa_object = accounts_list->data;
	authorizer = gdata_goa_authorizer_new (goa_object);
	service = gdata_picasaweb_service_new (GDATA_AUTHORIZER (authorizer));
	if (!gdata_service_is_authorized (GDATA_SERVICE (service)))
	{
		printf("Service not authorized\n");
		return -1;
	}

	/*Get all albums for the currently authenticated user.*/
	album_feed = gdata_picasaweb_service_query_all_albums (service,NULL,NULL,NULL,NULL,NULL,&error);
	error = NULL;
	if (error != NULL)
	{
		printf("Cannot query albums\n");
		g_error_free (error);
		return -1;
	}

	albums_list = gdata_feed_get_entries (album_feed);
	for (l = albums_list; l != NULL;l = l->next)
	  {
	    album = GDATA_PICASAWEB_ALBUM (l->data);
	    printf ("User : %s\n", gdata_picasaweb_album_get_user (album));
	    printf ("Album id : %s\n", gdata_picasaweb_album_get_id (album));
	    printf ("Number of photos : %d\n\n", gdata_picasaweb_album_get_num_photos(album));
	   
	  }

	g_object_unref (service);
	g_object_unref (authorizer);
	g_object_unref (album_feed);
	g_object_unref (client);
	g_object_unref (goa_object);
	g_list_free (albums_list);
	g_list_free (accounts_list);
}
