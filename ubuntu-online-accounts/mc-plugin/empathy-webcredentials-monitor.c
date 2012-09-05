#include "config.h"

#include <gio/gio.h>

#include <telepathy-glib/telepathy-glib.h>

#include "empathy-webcredentials-monitor.h"

G_DEFINE_TYPE (EmpathyWebcredentialsMonitor, empathy_webcredentials_monitor, G_TYPE_OBJECT)

#define WEBCRED_BUS_NAME "com.canonical.indicators.webcredentials"
#define WEBCRED_PATH "/com/canonical/indicators/webcredentials"
#define WEBCRED_IFACE "com.canonical.indicators.webcredentials"

enum
{
  FIRST_PROP = 1,
  N_PROPS
};

/*
enum
{
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];
*/

struct _EmpathyWebcredentialsMonitorPriv
{
  GDBusProxy *proxy;
};

static void
empathy_webcredentials_monitor_get_property (GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec)
{
  //EmpathyWebcredentialsMonitor *self = EMPATHY_WEBCREDENTIALS_MONITOR (object);

  switch (property_id)
    {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
empathy_webcredentials_monitor_set_property (GObject *object,
    guint property_id,
    const GValue *value,
    GParamSpec *pspec)
{
  //EmpathyWebcredentialsMonitor *self = EMPATHY_WEBCREDENTIALS_MONITOR (object);

  switch (property_id)
    {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
empathy_webcredentials_monitor_constructed (GObject *object)
{
  //EmpathyWebcredentialsMonitor *self = EMPATHY_WEBCREDENTIALS_MONITOR (object);
  void (*chain_up) (GObject *) =
      ((GObjectClass *) empathy_webcredentials_monitor_parent_class)->constructed;

  chain_up (object);
}

static void
empathy_webcredentials_monitor_dispose (GObject *object)
{
  EmpathyWebcredentialsMonitor *self = EMPATHY_WEBCREDENTIALS_MONITOR (object);
  void (*chain_up) (GObject *) =
      ((GObjectClass *) empathy_webcredentials_monitor_parent_class)->dispose;

  g_clear_object (&self->priv->proxy);

  chain_up (object);
}

static void
empathy_webcredentials_monitor_finalize (GObject *object)
{
  //EmpathyWebcredentialsMonitor *self = EMPATHY_WEBCREDENTIALS_MONITOR (object);
  void (*chain_up) (GObject *) =
      ((GObjectClass *) empathy_webcredentials_monitor_parent_class)->finalize;

  chain_up (object);
}

static void
empathy_webcredentials_monitor_class_init (
    EmpathyWebcredentialsMonitorClass *klass)
{
  GObjectClass *oclass = G_OBJECT_CLASS (klass);

  oclass->get_property = empathy_webcredentials_monitor_get_property;
  oclass->set_property = empathy_webcredentials_monitor_set_property;
  oclass->constructed = empathy_webcredentials_monitor_constructed;
  oclass->dispose = empathy_webcredentials_monitor_dispose;
  oclass->finalize = empathy_webcredentials_monitor_finalize;

  g_type_class_add_private (klass, sizeof (EmpathyWebcredentialsMonitorPriv));
}

static void
proxy_new_cb (GObject *source,
    GAsyncResult *result,
    gpointer user_data)
{
  EmpathyWebcredentialsMonitor *self;
  TpWeakRef *wr = user_data;
  GError *error = NULL;

  self = tp_weak_ref_dup_object (wr);
  if (self == NULL)
    goto out;

  self->priv->proxy = g_dbus_proxy_new_for_bus_finish (result, &error);
  if (self->priv->proxy == NULL)
    {
      g_debug ("Failed to create webcredentials proxy: %s", error->message);
      g_error_free (error);
      goto out;
    }

out:
  tp_weak_ref_destroy (wr);
  g_clear_object (&self);
}

static void
empathy_webcredentials_monitor_init (EmpathyWebcredentialsMonitor *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
      EMPATHY_TYPE_WEBCREDENTIALS_MONITOR, EmpathyWebcredentialsMonitorPriv);

  g_dbus_proxy_new_for_bus (G_BUS_TYPE_SESSION, G_DBUS_PROXY_FLAGS_NONE, NULL,
      WEBCRED_BUS_NAME, WEBCRED_PATH, WEBCRED_IFACE,
      NULL, proxy_new_cb, tp_weak_ref_new (self, NULL, NULL));
}

EmpathyWebcredentialsMonitor *
empathy_webcredentials_monitor_new (void)
{
  return g_object_new (EMPATHY_TYPE_WEBCREDENTIALS_MONITOR,
      NULL);
}
