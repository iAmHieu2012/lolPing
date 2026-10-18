// Native KDE Plasma 6 Wayland Ping & Emote Wheel (Qt6 + LayerShellQt)
// Built specifically for Arch Linux + KDE Plasma Wayland, reusing lolPing's assets.

#include <LayerShellQt/Window>
#include <QApplication>
#include <QAudioOutput>
#include <QCheckBox>
#include <QCloseEvent>
#include <QColor>
#include <QComboBox>
#include <QCoreApplication>
#include <QCursor>
#include <QDateTime>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDialog>
#include <QDir>
#include <QDoubleSpinBox>
#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QFont>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeyEvent>
#include <QLabel>
#include <QMediaPlayer>
#include <QMenu>
#include <QMouseEvent>
#include <QMovie>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPixmap>
#include <QProcess>
#include <QPushButton>
#include <QRadialGradient>
#include <QRegion>
#include <QScreen>
#include <QSlider>
#include <QSocketNotifier>
#include <QSoundEffect>
#include <QSpinBox>
#include <QStandardPaths>
#include <QSurfaceFormat>
#include <QSystemTrayIcon>
#include <QTabWidget>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>
#include <QWindow>

#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#include <X11/keysym.h>
#undef None
#undef Bool
#undef Status
#undef CursorShape
#undef Expose
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
#undef FontChange
#undef Unsorted

#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

namespace
{

  struct PingDef
  {
    QString id;
    QString name;
    QString nameVi;
    QString icon;
    QStringList art;
    QString sound;
    QColor color;
  };

  const std::vector<PingDef> &allPings()
  {
    static const std::vector<PingDef> kPings = {
        {"danger", "Danger", "Nguy hiểm (Rút lui)", "pingwheel_retreatrender", {"pingwheel_retreatrender"}, "SRP_12", QColor("#ff3b55")},
        {"push", "Push", "Đẩy đường", "push", {"pingwheel_pushrender"}, "SRP_9", QColor("#35d27a")},
        {"omw", "On My Way", "Đang tới", "omw", {"omw"}, "OnMyWay", QColor("#2e8bff")},
        {"allin", "All In", "Khô máu (All In)", "all_in", {"pingwheel_allin_render_01", "pingwheel_allin_render_02"}, "SRP_4", QColor("#f2b320")},
        {"assist", "Assist Me", "Hỗ trợ tôi", "assist_me", {"assist_me"}, "ComeHere", QColor("#56d43c")},
        {"needvision", "Need Vision", "Cần tầm nhìn", "need_vision", {"need_vision"}, "SRP_7", QColor("#56d43c")},
        {"missing", "Enemy Missing", "Kẻ địch mất tích (?)", "enemy_missing", {"enemy_missing"}, "MIA", QColor("#f2c518")},
        {"enemyvision", "Enemy Vision", "Có mắt địch", "pingwheel_enemyvisionrender", {"pingwheel_enemyvisionrender"}, "AreaIsWarded", QColor("#ff3b55")},
        {"generic", "Generic", "Ping cơ bản", "generic_ping", {"pingwheel_basicpingrender"}, "Base", QColor("#2e8bff")},
        {"bait", "Bait", "Nhử mồi (Bait)", "pingwheel_baitrender", {"pingwheel_baitrender"}, "SRP_11", QColor("#efbc12")},
        {"visioncleared", "Vision Cleared", "Đã phá mắt", "vision_cleared", {"pingwheel_visionclearedrender"}, "SRP_6", QColor("#2b80ff")},
        {"missingflip", "Upside-down Missing", "Chấm hỏi ngược (¿)", "enemy_missing_flipped", {"enemy_missing_flipped"}, "MIA", QColor("#f2c518")},
    };
    return kPings;
  }

  const PingDef *findPing(const QString &id)
  {
    for (const auto &p : allPings())
    {
      if (p.id == id)
        return &p;
    }
    return &allPings()[0];
  }

  struct EmoteDef
  {
    QString slug;
    QString name;
  };

  const std::vector<EmoteDef> &allEmotes()
  {
    static const std::vector<EmoteDef> kEmotes = {
        {"facepalm", "Facepalm"},
        {"unworthy", "Unworthy"},
        {"unworthy_cn", "Unworthy (CN)"},
        {"poro_snax", "Poro Snax"},
        {"sad_kitten", "Sad Kitten"},
        {"cheeky_poro", "Cheeky Poro"},
        {"angry_kitty", "Angry Kitty"},
        {"gg_heart", "GG <3"},
        {"bee_happy", "Bee Happy"},
        {"bee_mad", "Bee Mad"},
        {"bee_sad", "Bee Sad"},
        {"snoozy_poro", "Snoozy Poro"},
        {"wahaha", "Wahaha"},
        {"omg_love_it", "OMG I love it"},
        {"poro_ride", "Poro Ride"},
        {"dont_make_me_laugh", "Don't Make Me Laugh"},
        {"woah_dizzy", "Woah…Dizzy"},
        {"haha_hilarious", "Haha HILARIOUS"},
        {"gasp", "Gasp!"},
        {"super_approved", "Super-Approved"},
        {"pain", "PaiN…"},
        {"laughing_out_loud", "Laughing Out LOUD"},
        {"hats_off", "Hat's Off"},
        {"call_me", "Call Me!"},
        {"think_about_it", "Think About It"},
        {"unimpressed", "Animated Unimpressed!"},
        {"did_you_just", "Animated Did You Just?"},
        {"nice_try", "Nice Try"},
        {"nice", "Nice"},
        {"despair", "Despair"},
        {"does_not_compute", "Does Not Compute"},
        {"come_at_me", "Come at Me"},
        {"how_could_you", "How Could You"},
        {"scout_approved", "Scout-Approved"},
        {"maybe_next_time", "Maybe Next Time"},
        {"okay", "Okay"},
        {"good_job_buddy", "Good Job, Buddy!"},
        {"teamwork", "Teamwork!"},
        {"i_will_destroy_you", "I will destroy you"},
        {"not_today", "Not Today!"},
        {"oh", "Oh..."},
        {"come_again", "Come again?"},
        {"magpie_oopsie", "Magpie Oopsie"},
        {"such_foolishness", "Such Foolishness"},
    };
    return kEmotes;
  }

  enum class TriggerMod
  {
    Alt = 0,
    Ctrl = 1,
    Shift = 2,
    Super = 3,
    Disabled = 4
  };
  enum class ActiveWheel
  {
    None,
    Ping,
    Emote
  };

  struct AppSettings
  {
    TriggerMod pingTrigger = TriggerMod::Alt;
    TriggerMod emoteTrigger = TriggerMod::Ctrl;
    bool clickPing = true;
    bool clickEmote = true;
    int volume = 80;     // 0..100
    int wheelSize = 460; // px
    int pingSize = 115;  // px
    double pingDuration = 3.2;
    QString clickPingId = "generic";
    QString clickEmoteSlug = "facepalm";
    std::array<QString, 8> pingWheel = {
        "danger", "push", "omw", "allin", "assist", "needvision", "missing", "enemyvision"};
    std::array<QString, 8> emoteWheel = {
        "unworthy_cn", "poro_snax", "sad_kitten", "cheeky_poro",
        "angry_kitty", "gg_heart", "bee_happy", "bee_mad"};

    static QString configPath()
    {
      QString dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
      if (dir.isEmpty())
        dir = QDir::homePath() + "/.config/lolPing";
      QDir().mkpath(dir);
      return dir + "/kde-wayland.json";
    }

    void load()
    {
      QFile f(configPath());
      if (!f.open(QIODevice::ReadOnly))
        return;
      QJsonObject o = QJsonDocument::fromJson(f.readAll()).object();
      if (o.contains("pingTrigger"))
        pingTrigger = static_cast<TriggerMod>(std::clamp(o["pingTrigger"].toInt(0), 0, 3));
      if (o.contains("emoteTrigger"))
        emoteTrigger = static_cast<TriggerMod>(std::clamp(o["emoteTrigger"].toInt(1), 0, 4));
      if (o.contains("clickPing"))
        clickPing = o["clickPing"].toBool(true);
      if (o.contains("clickEmote"))
        clickEmote = o["clickEmote"].toBool(true);
      if (o.contains("volume"))
        volume = std::clamp(o["volume"].toInt(80), 0, 100);
      if (o.contains("wheelSize"))
        wheelSize = std::clamp(o["wheelSize"].toInt(460), 280, 720);
      if (o.contains("pingSize"))
        pingSize = std::clamp(o["pingSize"].toInt(115), 64, 220);
      if (o.contains("pingDuration"))
        pingDuration = std::clamp(o["pingDuration"].toDouble(3.2), 1.0, 6.0);
      if (o.contains("clickPingId"))
        clickPingId = o["clickPingId"].toString("generic");
      if (o.contains("clickEmoteSlug"))
        clickEmoteSlug = o["clickEmoteSlug"].toString("facepalm");
      if (o["pingWheel"].isArray())
      {
        QJsonArray arr = o["pingWheel"].toArray();
        for (int i = 0; i < 8 && i < arr.size(); ++i)
          pingWheel[i] = arr[i].toString(pingWheel[i]);
      }
      if (o["emoteWheel"].isArray())
      {
        QJsonArray arr = o["emoteWheel"].toArray();
        for (int i = 0; i < 8 && i < arr.size(); ++i)
          emoteWheel[i] = arr[i].toString(emoteWheel[i]);
      }
    }

    void save() const
    {
      QJsonObject o;
      o["pingTrigger"] = static_cast<int>(pingTrigger);
      o["emoteTrigger"] = static_cast<int>(emoteTrigger);
      o["clickPing"] = clickPing;
      o["clickEmote"] = clickEmote;
      o["volume"] = volume;
      o["wheelSize"] = wheelSize;
      o["pingSize"] = pingSize;
      o["pingDuration"] = pingDuration;
      o["clickPingId"] = clickPingId;
      o["clickEmoteSlug"] = clickEmoteSlug;
      QJsonArray pw, ew;
      for (int i = 0; i < 8; ++i)
      {
        pw.append(pingWheel[i]);
        ew.append(emoteWheel[i]);
      }
      o["pingWheel"] = pw;
      o["emoteWheel"] = ew;
      QFile f(configPath());
      if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
      {
        f.write(QJsonDocument(o).toJson(QJsonDocument::Indented));
      }
    }
  };

  // Ensures KWin Wayland forwards modifier/key events to XWayland so global triggers work without root/sudo.
  void ensureKWinWaylandSupport()
  {
    QProcess::execute("kwriteconfig6", {"--notify", "--file", "kwinrc", "--group", "Xwayland", "--key", "XwaylandEavesdrops", "All"});
    QProcess::execute("kwriteconfig6", {"--notify", "--file", "kwinrc", "--group", "Xwayland", "--key", "XwaylandEavesdropsMouse", "true"});
    QDBusMessage msg = QDBusMessage::createMethodCall("org.kde.KWin", "/KWin", "org.kde.KWin", "reconfigure");
    QDBusConnection::sessionBus().asyncCall(msg);
    QDBusMessage sig = QDBusMessage::createSignal("/KWin", "org.kde.kwin", "reloadConfig");
    QDBusConnection::sessionBus().send(sig);
  }

  // Geometry matching src/shared/geometry.ts
  constexpr double kBaseSize = 600.0;
  constexpr double kInnerR = 86.0;
  constexpr double kOuterR = 188.0;
  constexpr double kExtR = 288.0;
  constexpr double kIconR = 137.0;
  constexpr double kIconSize = 48.0;

  int pickSlice(double dx, double dy, double deadZone)
  {
    if (std::hypot(dx, dy) < deadZone)
      return -1;
    double deg = std::fmod((std::atan2(dy, dx) * 180.0 / M_PI) + 90.0 + 22.5 + 360.0, 360.0);
    return static_cast<int>(std::floor(deg / 45.0)) % 8;
  }

  QPainterPath makeSectorPath(double r0, double r1, double a0Deg, double a1Deg)
  {
    QPainterPath path;
    double a0Rad = a0Deg * M_PI / 180.0;
    path.moveTo(r0 * std::cos(a0Rad), r0 * std::sin(a0Rad));
    path.lineTo(r1 * std::cos(a0Rad), r1 * std::sin(a0Rad));
    QRectF outerRect(-r1, -r1, r1 * 2.0, r1 * 2.0);
    // QPainterPath::arcTo angles are counter-clockwise in degrees starting at 3 o'clock
    path.arcTo(outerRect, -a0Deg, -(a1Deg - a0Deg));
    QRectF innerRect(-r0, -r0, r0 * 2.0, r0 * 2.0);
    path.arcTo(innerRect, -a1Deg, (a1Deg - a0Deg));
    path.closeSubpath();
    return path;
  }

  QImage makeSepiaIcon(const QImage &src)
  {
    QImage out = src.convertToFormat(QImage::Format_ARGB32);
    for (int y = 0; y < out.height(); ++y)
    {
      QRgb *line = reinterpret_cast<QRgb *>(out.scanLine(y));
      for (int x = 0; x < out.width(); ++x)
      {
        int a = qAlpha(line[x]);
        if (a == 0)
          continue;
        int gray = qGray(line[x]);
        int r = std::min(255, static_cast<int>(gray * 1.28 + 28));
        int g = std::min(255, static_cast<int>(gray * 1.08 + 14));
        int b = std::min(255, static_cast<int>(gray * 0.72));
        line[x] = qRgba(r, g, b, a);
      }
    }
    return out;
  }

  class AssetStore
  {
  public:
    QString assetsDir;
    std::map<QString, QPixmap> textures;
    std::map<QString, QPixmap> sepiaTextures;
    std::map<QString, QPixmap> emoteIcons;
    std::map<QString, std::vector<std::unique_ptr<QSoundEffect>>> wavPool;
    std::vector<std::pair<std::unique_ptr<QMediaPlayer>, std::unique_ptr<QAudioOutput>>> oggPool;
    size_t oggIdx = 0;

    explicit AssetStore(const QString &rootDir) : assetsDir(rootDir)
    {
      QDir texDir(assetsDir + "/textures");
      for (const QString &file : texDir.entryList({"*.png"}, QDir::Files))
      {
        QString key = QFileInfo(file).completeBaseName();
        QImage img(texDir.filePath(file));
        if (!img.isNull())
        {
          textures[key] = QPixmap::fromImage(img);
          sepiaTextures[key] = QPixmap::fromImage(makeSepiaIcon(img));
        }
      }
      QDir emoteDir(assetsDir + "/emotes");
      for (const QString &file : emoteDir.entryList({"*.png"}, QDir::Files))
      {
        QString key = QFileInfo(file).completeBaseName();
        QPixmap px(emoteDir.filePath(file));
        if (!px.isNull())
          emoteIcons[key] = px;
      }
      QDir sndDir(assetsDir + "/sounds");
      for (const QString &file : sndDir.entryList({"*.wav"}, QDir::Files))
      {
        QString key = QFileInfo(file).completeBaseName();
        auto &vec = wavPool[key];
        for (int i = 0; i < 3; ++i)
        {
          auto sfx = std::make_unique<QSoundEffect>();
          sfx->setSource(QUrl::fromLocalFile(sndDir.filePath(file)));
          vec.push_back(std::move(sfx));
        }
      }
      for (int i = 0; i < 4; ++i)
      {
        auto player = std::make_unique<QMediaPlayer>();
        auto audio = std::make_unique<QAudioOutput>();
        player->setAudioOutput(audio.get());
        oggPool.emplace_back(std::move(player), std::move(audio));
      }
    }

    const QPixmap &texture(const QString &name, bool sepia = false) const
    {
      const auto &map = sepia ? sepiaTextures : textures;
      auto it = map.find(name);
      if (it != map.end())
        return it->second;
      auto fb = map.find("generic_ping");
      if (fb != map.end())
        return fb->second;
      static QPixmap empty;
      return empty;
    }

    const QPixmap &emoteIcon(const QString &slug) const
    {
      auto it = emoteIcons.find(slug);
      if (it != emoteIcons.end())
        return it->second;
      return texture("generic_ping", false);
    }

    void playWav(const QString &name, int volumePercent)
    {
      if (volumePercent <= 0)
        return;
      auto it = wavPool.find(name);
      if (it == wavPool.end() || it->second.empty())
        return;
      float vol = std::clamp(volumePercent / 100.0f, 0.0f, 1.0f);
      for (auto &sfx : it->second)
      {
        if (!sfx->isPlaying())
        {
          sfx->setVolume(vol);
          sfx->play();
          return;
        }
      }
      auto &sfx = it->second[0];
      sfx->setVolume(vol);
      sfx->play();
    }

    void playEmoteOgg(const QString &slug, int volumePercent)
    {
      if (volumePercent <= 0 || oggPool.empty())
        return;
      QString path = assetsDir + "/emotes/" + slug + ".ogg";
      if (!QFileInfo::exists(path))
        return;
      auto &[player, audio] = oggPool[oggIdx % oggPool.size()];
      ++oggIdx;
      audio->setVolume(std::clamp(volumePercent / 100.0f, 0.0f, 1.0f));
      player->stop();
      player->setSource(QUrl::fromLocalFile(path));
      player->play();
    }
  };

  struct ActivePing
  {
    const PingDef *def = nullptr;
    QPointF pos;
    qint64 startMs = 0;
    double durationS = 3.2;
    int sizePx = 115;
  };

  struct ActiveEmote
  {
    QString slug;
    QPointF pos;
    qint64 startMs = 0;
    double durationS = 3.0;
    int sizePx = 140;
    std::unique_ptr<QMovie> movie;
  };

  struct ActiveToast
  {
    QString title;
    QString body;
    qint64 startMs = 0;
    double durationS = 2.6;
  };

  class OverlayWindow : public QWidget
  {
  public:
    AppSettings &settings;
    AssetStore &assets;
    LayerShellQt::Window *layerWin = nullptr;

    bool paused = false;
    bool inputArmed = false;
    ActiveWheel armedKind = ActiveWheel::None;

    bool mousePressed = false;
    bool wheelOpen = false;
    ActiveWheel openKind = ActiveWheel::None;
    QPointF pressPos;
    QPointF curPos;
    int hotSlice = -1;

    std::vector<ActivePing> pings;
    std::vector<ActiveEmote> emotes;
    std::vector<ActiveToast> toasts;
    QElapsedTimer clock;
    QTimer animTimer;

    OverlayWindow(AppSettings &s, AssetStore &a) : settings(s), assets(a)
    {
      clock.start();
      setAttribute(Qt::WA_TranslucentBackground, true);
      setAttribute(Qt::WA_NoSystemBackground, true);
      setAttribute(Qt::WA_ShowWithoutActivating, true);
      setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
      setMouseTracking(true);

      if (QScreen *scr = QGuiApplication::primaryScreen())
      {
        setGeometry(scr->geometry());
      }

      winId(); // create native QWindow before attaching LayerShellQt
      if (QWindow *w = windowHandle())
      {
        layerWin = LayerShellQt::Window::get(w);
        if (layerWin)
        {
          layerWin->setLayer(LayerShellQt::Window::LayerOverlay);
          layerWin->setScope("lolping-kde-overlay");
          layerWin->setAnchors(LayerShellQt::Window::Anchors(
              LayerShellQt::Window::AnchorTop | LayerShellQt::Window::AnchorBottom |
              LayerShellQt::Window::AnchorLeft | LayerShellQt::Window::AnchorRight));
          layerWin->setExclusiveZone(-1);
          layerWin->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivityNone);
          layerWin->setActivateOnShow(false);
        }
        w->setFlag(Qt::WindowTransparentForInput, true);
      }

      QObject::connect(&animTimer, &QTimer::timeout, this, [this]()
                       { onTick(); });
      animTimer.setInterval(16); // ~60 FPS when animations are active
    }

    void ensureAnimTimer()
    {
      bool need = wheelOpen || !pings.empty() || !emotes.empty() || !toasts.empty();
      if (need && !animTimer.isActive())
        animTimer.start();
      else if (!need && animTimer.isActive())
        animTimer.stop();
    }

    void setArmed(bool arm, ActiveWheel kind)
    {
      if (paused)
        arm = false;
      if (!arm)
      {
        if (wheelOpen || mousePressed)
        {
          cancelGesture();
        }
        armedKind = ActiveWheel::None;
      }
      else
      {
        armedKind = kind;
      }
      if (inputArmed == arm)
        return;
      inputArmed = arm;
      if (QWindow *w = windowHandle())
      {
        w->setFlag(Qt::WindowTransparentForInput, !arm);
        w->requestUpdate();
      }
      repaint(0, 0, 2, 2);
    }

    void togglePause()
    {
      paused = !paused;
      if (paused)
      {
        setArmed(false, ActiveWheel::None);
        showToast("lolPing Paused", "Resume anytime from the system tray icon");
      }
      else
      {
        showToast("lolPing Resumed", "Hold Alt + Left Drag to open the Ping Wheel");
      }
    }

    void showToast(const QString &title, const QString &body)
    {
      ActiveToast t;
      t.title = title;
      t.body = body;
      t.startMs = clock.elapsed();
      toasts.push_back(t);
      ensureAnimTimer();
      update();
    }

    void spawnPingAt(const QString &id, QPointF pos)
    {
      const PingDef *def = findPing(id);
      if (!def)
        return;
      while (pings.size() >= 40)
        pings.erase(pings.begin());
      ActivePing p;
      p.def = def;
      p.pos = pos;
      p.startMs = clock.elapsed();
      p.durationS = settings.pingDuration;
      p.sizePx = settings.pingSize;
      pings.push_back(p);
      assets.playWav(def->sound, settings.volume);
      ensureAnimTimer();
      update();
    }

    void spawnEmoteAt(const QString &slug, QPointF pos)
    {
      // Replace existing emote
      emotes.clear();
      ActiveEmote e;
      e.slug = slug;
      e.pos = pos;
      e.startMs = clock.elapsed();
      e.durationS = 3.0;
      e.sizePx = std::max(120, settings.pingSize + 20);
      QString webpPath = assets.assetsDir + "/emotes/" + slug + ".webp";
      if (QFileInfo::exists(webpPath))
      {
        e.movie = std::make_unique<QMovie>(webpPath);
        e.movie->setCacheMode(QMovie::CacheAll);
        e.movie->start();
      }
      emotes.push_back(std::move(e));
      assets.playEmoteOgg(slug, settings.volume);
      ensureAnimTimer();
      update();
    }

    void cancelGesture()
    {
      mousePressed = false;
      if (wheelOpen)
      {
        wheelOpen = false;
        openKind = ActiveWheel::None;
        hotSlice = -1;
        ensureAnimTimer();
        update();
      }
    }

  protected:
    void mousePressEvent(QMouseEvent *ev) override
    {
      if (!inputArmed || paused)
        return;
      if (ev->button() == Qt::RightButton)
      {
        cancelGesture();
        setArmed(false, ActiveWheel::None);
        return;
      }
      if (ev->button() == Qt::LeftButton)
      {
        mousePressed = true;
        pressPos = ev->position();
        curPos = pressPos;
        hotSlice = -1;
        openKind = armedKind;
        update();
      }
    }

    void mouseMoveEvent(QMouseEvent *ev) override
    {
      curPos = ev->position();
      if (!mousePressed)
        return;
      double dist = std::hypot(curPos.x() - pressPos.x(), curPos.y() - pressPos.y());
      if (!wheelOpen && dist >= 8.0)
      {
        wheelOpen = true;
        ensureAnimTimer();
      }
      if (wheelOpen)
      {
        double scale = settings.wheelSize / kBaseSize;
        int slice = pickSlice(curPos.x() - pressPos.x(), curPos.y() - pressPos.y(), kInnerR * scale);
        if (slice != hotSlice)
        {
          hotSlice = slice;
          if (hotSlice >= 0)
          {
            assets.playWav("button", std::max(25, settings.volume / 2));
          }
        }
        update();
      }
    }

    void mouseReleaseEvent(QMouseEvent *ev) override
    {
      if (ev->button() != Qt::LeftButton || !mousePressed)
        return;
      curPos = ev->position();
      mousePressed = false;
      if (wheelOpen)
      {
        double scale = settings.wheelSize / kBaseSize;
        int slice = pickSlice(curPos.x() - pressPos.x(), curPos.y() - pressPos.y(), kInnerR * scale);
        ActiveWheel kind = openKind;
        wheelOpen = false;
        openKind = ActiveWheel::None;
        hotSlice = -1;
        if (slice >= 0 && slice < 8)
        {
          if (kind == ActiveWheel::Ping)
          {
            spawnPingAt(settings.pingWheel[slice], pressPos);
          }
          else if (kind == ActiveWheel::Emote)
          {
            spawnEmoteAt(settings.emoteWheel[slice], pressPos);
          }
        }
      }
      else
      {
        // Short click while trigger held
        if (armedKind == ActiveWheel::Ping && settings.clickPing)
        {
          spawnPingAt(settings.clickPingId, pressPos);
        }
        else if (armedKind == ActiveWheel::Emote && settings.clickEmote)
        {
          spawnEmoteAt(settings.clickEmoteSlug, pressPos);
        }
      }
      ensureAnimTimer();
      update();
    }

    void onTick()
    {
      qint64 now = clock.elapsed();
      pings.erase(
          std::remove_if(pings.begin(), pings.end(),
                         [now](const ActivePing &p)
                         { return (now - p.startMs) > p.durationS * 1000.0 + 100; }),
          pings.end());
      emotes.erase(
          std::remove_if(emotes.begin(), emotes.end(),
                         [now](const ActiveEmote &e)
                         { return (now - e.startMs) > e.durationS * 1000.0 + 100; }),
          emotes.end());
      toasts.erase(
          std::remove_if(toasts.begin(), toasts.end(),
                         [now](const ActiveToast &t)
                         { return (now - t.startMs) > t.durationS * 1000.0; }),
          toasts.end());
      ensureAnimTimer();
      update();
    }

    void paintEvent(QPaintEvent *) override
    {
      QPainter p(this);
      p.setRenderHint(QPainter::Antialiasing, true);
      p.setRenderHint(QPainter::SmoothPixmapTransform, true);

      // When inputArmed is true on Wayland, an alpha of 1/255 across the surface guarantees KWin routes pointer events
      // to this LayerOverlay surface across the whole screen while remaining visually completely invisible.
      if (inputArmed)
      {
        p.fillRect(rect(), QColor(0, 0, 0, 1));
      }

      qint64 now = clock.elapsed();
      for (const auto &ping : pings)
        drawPing(p, ping, now);
      for (const auto &emote : emotes)
        drawEmote(p, emote, now);
      if (wheelOpen)
        drawWheel(p);
      for (size_t i = 0; i < toasts.size(); ++i)
        drawToast(p, toasts[i], now, static_cast<int>(i));
    }

  private:
    void drawWheel(QPainter &p)
    {
      p.save();
      p.translate(pressPos);
      double scale = settings.wheelSize / kBaseSize;
      p.scale(scale, scale);

      // 1. Outer extensions on N, E, S, W (i % 2 == 0)
      for (int i = 0; i < 8; ++i)
      {
        if (i % 2 != 0)
          continue;
        double mid = -90.0 + i * 45.0;
        QPainterPath extPath = makeSectorPath(kOuterR, kExtR, mid - 22.5, mid + 22.5);
        QRadialGradient extGrad(0, 0, kExtR);
        extGrad.setColorAt(kOuterR / kExtR, QColor(92, 96, 102, 140));
        extGrad.setColorAt(1.0, QColor(92, 96, 102, 0));
        p.fillPath(extPath, extGrad);
      }

      // 2. Main dark ring
      QPainterPath ringPath = makeSectorPath(kInnerR, kOuterR, 0.0, 360.0);
      QRadialGradient ringGrad(0, 0, kOuterR);
      ringGrad.setColorAt(kInnerR / kOuterR, QColor(42, 45, 49, 235));
      ringGrad.setColorAt(1.0, QColor(74, 78, 84, 235));
      p.fillPath(ringPath, ringGrad);

      // 3. Hot wedge highlight + divider lines
      for (int i = 0; i < 8; ++i)
      {
        double mid = -90.0 + i * 45.0;
        if (i == hotSlice)
        {
          double reach = (i % 2 == 0) ? kExtR : kOuterR;
          QPainterPath hotPath = makeSectorPath(kInnerR, reach, mid - 22.5, mid + 22.5);
          QRadialGradient hotGrad(0, 0, kExtR);
          hotGrad.setColorAt(kInnerR / kExtR, QColor(138, 143, 150, 165));
          hotGrad.setColorAt(kOuterR / kExtR, QColor(154, 160, 168, 155));
          hotGrad.setColorAt(1.0, QColor(154, 160, 168, 0));
          p.fillPath(hotPath, hotGrad);
        }
        double lineRad = (mid - 22.5) * M_PI / 180.0;
        p.setPen(QPen(QColor(160, 138, 92, 120), 1.4));
        p.drawLine(QPointF(kInnerR * std::cos(lineRad), kInnerR * std::sin(lineRad)),
                   QPointF(kOuterR * std::cos(lineRad), kOuterR * std::sin(lineRad)));
      }

      // 4. Slice icons
      for (int i = 0; i < 8; ++i)
      {
        double rad = (-90.0 + i * 45.0) * M_PI / 180.0;
        double ix = kIconR * std::cos(rad);
        double iy = kIconR * std::sin(rad);
        bool isHot = (i == hotSlice);
        double sz = isHot ? kIconSize * 1.22 : kIconSize;
        QRectF iconRect(ix - sz / 2.0, iy - sz / 2.0, sz, sz);

        if (openKind == ActiveWheel::Ping)
        {
          const PingDef *def = findPing(settings.pingWheel[i]);
          const QPixmap &px = assets.texture(def->icon, !isHot);
          if (!px.isNull())
            p.drawPixmap(iconRect.toRect(), px);
        }
        else
        {
          const QPixmap &px = assets.emoteIcon(settings.emoteWheel[i]);
          if (!px.isNull())
          {
            p.save();
            QPainterPath clip;
            clip.addEllipse(iconRect);
            p.setClipPath(clip);
            p.drawPixmap(iconRect.toRect(), px);
            p.restore();
          }
        }
      }

      // 5. Center hub
      p.setPen(QPen(QColor(26, 26, 26, 155), 6.0));
      p.setBrush(Qt::NoBrush);
      p.drawEllipse(QPointF(0, 0), kInnerR + 4.0, kInnerR + 4.0);

      QRadialGradient coreGrad(0, -10, kInnerR);
      coreGrad.setColorAt(0.0, QColor("#2c5d60"));
      coreGrad.setColorAt(1.0, QColor("#11292c"));
      p.setPen(QPen(QColor("#c8aa6e"), 3.0));
      p.setBrush(coreGrad);
      p.drawEllipse(QPointF(0, 0), kInnerR, kInnerR);

      p.setPen(QPen(QColor(138, 116, 72, 155), 1.0));
      p.setBrush(Qt::NoBrush);
      p.drawEllipse(QPointF(0, 0), kInnerR - 7.0, kInnerR - 7.0);

      // Hub icon & label
      QRectF hubRect(-20, -58, 40, 40);
      if (openKind == ActiveWheel::Ping)
      {
        const PingDef *centerDef = findPing(settings.clickPingId);
        const QPixmap &hubPx = assets.texture(centerDef->icon, false);
        if (!hubPx.isNull())
          p.drawPixmap(hubRect.toRect(), hubPx);
      }
      else
      {
        const QPixmap &hubPx = assets.emoteIcon(settings.clickEmoteSlug);
        if (!hubPx.isNull())
        {
          p.save();
          QPainterPath clip;
          clip.addEllipse(hubRect);
          p.setClipPath(clip);
          p.drawPixmap(hubRect.toRect(), hubPx);
          p.restore();
        }
      }

      QFont centerFont("Sans Serif", 11, QFont::DemiBold);
      p.setFont(centerFont);
      p.setPen(QColor("#1fa9e0"));
      QString centerText = (openKind == ActiveWheel::Ping) ? "PING" : "EMOTE";
      if (hotSlice >= 0 && hotSlice < 8)
      {
        if (openKind == ActiveWheel::Ping)
        {
          centerText = findPing(settings.pingWheel[hotSlice])->name.toUpper();
        }
        else
        {
          QString slug = settings.emoteWheel[hotSlice];
          centerText = slug.toUpper();
          for (const auto &ed : allEmotes())
          {
            if (ed.slug == slug)
            {
              centerText = ed.name.toUpper();
              break;
            }
          }
        }
      }
      p.drawText(QRectF(-80, -8, 160, 24), Qt::AlignCenter, centerText);

      QFont backFont("Sans Serif", 7, QFont::DemiBold);
      p.setFont(backFont);
      p.setPen(QColor("#6c7a7c"));
      p.drawText(QRectF(-40, 38, 80, 18), Qt::AlignCenter, "ESC / R-CLICK");

      // Direction line from center to cursor when dragging
      double dx = (curPos.x() - pressPos.x()) / scale;
      double dy = (curPos.y() - pressPos.y()) / scale;
      if (std::hypot(dx, dy) > 12.0)
      {
        p.setPen(QPen(QColor(200, 170, 110, 190), 2.2));
        double clampLen = std::min(std::hypot(dx, dy), kOuterR - 8.0);
        double ang = std::atan2(dy, dx);
        p.drawLine(QPointF(0, 0), QPointF(clampLen * std::cos(ang), clampLen * std::sin(ang)));
      }

      p.restore();
    }

    void drawPing(QPainter &p, const ActivePing &ping, qint64 now)
    {
      double elapsedS = (now - ping.startMs) / 1000.0;
      if (elapsedS < 0.0 || elapsedS > ping.durationS)
        return;
      double lifeProgress = elapsedS / ping.durationS;
      double lifeAlpha = (lifeProgress < 0.85) ? 1.0 : std::max(0.0, (1.0 - lifeProgress) / 0.15);

      p.save();
      p.translate(ping.pos);
      double sz = ping.sizePx;
      QColor c = ping.def->color;

      // 1. Ground glow
      double gAlpha = (lifeProgress < 0.1) ? (lifeProgress / 0.1) * 0.75 : (1.0 - lifeProgress) * 0.75;
      QRadialGradient groundGrad(0, sz * 0.32, sz * 0.45);
      QColor gc = c;
      gc.setAlphaF(std::clamp(gAlpha, 0.0, 1.0));
      groundGrad.setColorAt(0.0, gc);
      groundGrad.setColorAt(1.0, QColor(c.red(), c.green(), c.blue(), 0));
      p.save();
      p.translate(0, sz * 0.32);
      p.scale(1.0, 0.32);
      p.setPen(Qt::NoPen);
      p.setBrush(groundGrad);
      p.drawEllipse(QPointF(0, 0), sz * 0.45, sz * 0.45);
      p.restore();

      // 2. Three staggered expanding shockwave rings
      const double ringDelays[3] = {0.0, 0.22, 1.40};
      for (double delay : ringDelays)
      {
        double rt = elapsedS - delay;
        if (rt < 0.0 || rt > 0.65)
          continue;
        double rp = rt / 0.65;
        double rScale = 0.25 + rp * 1.10;
        double rAlpha = (1.0 - rp) * 0.92;
        QColor rc = c;
        rc.setAlphaF(std::clamp(rAlpha, 0.0, 1.0));
        p.setPen(QPen(rc, 3.0));
        p.setBrush(Qt::NoBrush);
        double rad = sz * 0.54 * rScale;
        p.drawEllipse(QPointF(0, 0), rad, rad);
      }

      // 3. Pop-in + bobbing Ping icon
      double popScale = 1.0;
      if (elapsedS < 0.16)
      {
        popScale = 0.4 + (elapsedS / 0.16) * 0.78;
      }
      else if (elapsedS < 0.28)
      {
        popScale = 1.18 - ((elapsedS - 0.16) / 0.12) * 0.18;
      }
      double bobY = (elapsedS > 0.28) ? std::sin((elapsedS - 0.28) * 3.9) * 5.0 : 0.0;

      p.setOpacity(lifeAlpha);
      p.translate(0, -sz * 0.23 + bobY);
      p.scale(popScale, popScale);

      // Outer colored glow behind icon
      QRadialGradient aura(0, 0, sz * 0.55);
      QColor ac = c;
      ac.setAlphaF(0.38);
      aura.setColorAt(0.0, ac);
      aura.setColorAt(1.0, QColor(c.red(), c.green(), c.blue(), 0));
      p.setPen(Qt::NoPen);
      p.setBrush(aura);
      p.drawEllipse(QPointF(0, 0), sz * 0.55, sz * 0.55);

      QRectF artRect(-sz * 0.5, -sz * 0.5, sz, sz);
      if (ping.def->art.size() == 2)
      {
        double slide = std::min(1.0, elapsedS / 0.22);
        double offset = (1.0 - slide) * sz * 0.45;
        const QPixmap &px1 = assets.texture(ping.def->art[0], false);
        const QPixmap &px2 = assets.texture(ping.def->art[1], false);
        if (!px1.isNull())
          p.drawPixmap(artRect.translated(offset, -offset).toRect(), px1);
        if (!px2.isNull())
          p.drawPixmap(artRect.translated(-offset, -offset).toRect(), px2);
      }
      else if (!ping.def->art.isEmpty())
      {
        const QPixmap &px = assets.texture(ping.def->art[0], false);
        if (!px.isNull())
          p.drawPixmap(artRect.toRect(), px);
      }

      p.restore();
    }

    void drawEmote(QPainter &p, const ActiveEmote &emote, qint64 now)
    {
      double elapsedS = (now - emote.startMs) / 1000.0;
      if (elapsedS < 0.0 || elapsedS > emote.durationS)
        return;
      double lifeProgress = elapsedS / emote.durationS;
      double alpha = (lifeProgress < 0.86) ? 1.0 : std::max(0.0, (1.0 - lifeProgress) / 0.14);
      double scale = 1.0;
      if (elapsedS < 0.18)
        scale = 0.35 + (elapsedS / 0.18) * 0.80;
      else if (elapsedS < 0.30)
        scale = 1.15 - ((elapsedS - 0.18) / 0.12) * 0.15;

      p.save();
      p.translate(emote.pos);
      p.setOpacity(alpha);
      p.scale(scale, scale);

      double sz = emote.sizePx * 1.25;
      QRectF r(-sz * 0.5, -sz * 0.65, sz, sz);
      if (emote.movie && emote.movie->isValid())
      {
        QPixmap frame = emote.movie->currentPixmap();
        if (!frame.isNull())
          p.drawPixmap(r.toRect(), frame);
      }
      else
      {
        const QPixmap &px = assets.emoteIcon(emote.slug);
        if (!px.isNull())
          p.drawPixmap(r.toRect(), px);
      }
      p.restore();
    }

    void drawToast(QPainter &p, const ActiveToast &toast, qint64 now, int index)
    {
      double elapsedS = (now - toast.startMs) / 1000.0;
      if (elapsedS < 0.0 || elapsedS > toast.durationS)
        return;
      double alpha = 1.0;
      if (elapsedS < 0.2)
        alpha = elapsedS / 0.2;
      else if (elapsedS > toast.durationS - 0.25)
        alpha = (toast.durationS - elapsedS) / 0.25;

      p.save();
      p.setOpacity(std::clamp(alpha, 0.0, 1.0));
      int w = 360;
      int h = 58;
      int x = width() - w - 24;
      int y = height() - h - 72 - index * (h + 10);
      QRectF box(x, y, w, h);

      p.setPen(QPen(QColor("#c8aa6e"), 2.0));
      p.setBrush(QColor(7, 19, 26, 235));
      p.drawRoundedRect(box, 6, 6);

      const QPixmap &icon = assets.texture("generic_ping", false);
      if (!icon.isNull())
      {
        p.drawPixmap(QRect(x + 12, y + 13, 32, 32), icon);
      }

      p.setPen(QColor("#f0e6d2"));
      p.setFont(QFont("Sans Serif", 10, QFont::Bold));
      p.drawText(QRectF(x + 54, y + 8, w - 64, 20), Qt::AlignLeft | Qt::AlignVCenter, toast.title);

      p.setPen(QColor("#a09b8c"));
      p.setFont(QFont("Sans Serif", 9));
      p.drawText(QRectF(x + 54, y + 28, w - 64, 22), Qt::AlignLeft | Qt::AlignVCenter, toast.body);
      p.restore();
    }
  };

  // Listens for global modifier keys via KWin's XWayland raw key stream (no root needed) AND /dev/input/event* if accessible.
  class GlobalKeyWatcher : public QObject
  {
  public:
    OverlayWindow &overlay;
    AppSettings &settings;

    Display *dpy = nullptr;
    int xiOpcode = 0;
    std::unique_ptr<QSocketNotifier> x11Notifier;
    std::vector<int> evdevFds;
    std::vector<std::unique_ptr<QSocketNotifier>> evdevNotifiers;

    bool altDown = false;
    bool ctrlDown = false;
    bool shiftDown = false;
    bool superDown = false;
    bool foreignKeyDown = false;

    GlobalKeyWatcher(OverlayWindow &ov, AppSettings &s) : overlay(ov), settings(s)
    {
      setupX11RawKeys();
      setupEvdevKeys();
    }

    ~GlobalKeyWatcher() override
    {
      for (int fd : evdevFds)
        ::close(fd);
      if (dpy)
        XCloseDisplay(dpy);
    }

    bool isModHeld(TriggerMod m) const
    {
      switch (m)
      {
      case TriggerMod::Alt:
        return altDown;
      case TriggerMod::Ctrl:
        return ctrlDown;
      case TriggerMod::Shift:
        return shiftDown;
      case TriggerMod::Super:
        return superDown;
      case TriggerMod::Disabled:
        return false;
      }
      return false;
    }

    void updateArmedState()
    {
      if (foreignKeyDown)
      {
        overlay.setArmed(false, ActiveWheel::None);
        return;
      }
      if (isModHeld(settings.pingTrigger))
      {
        overlay.setArmed(true, ActiveWheel::Ping);
      }
      else if (isModHeld(settings.emoteTrigger))
      {
        overlay.setArmed(true, ActiveWheel::Emote);
      }
      else
      {
        overlay.setArmed(false, ActiveWheel::None);
      }
    }

    void onKeySym(KeySym sym, bool down)
    {
      if (sym == XK_Alt_L || sym == XK_Alt_R || sym == XK_Meta_L || sym == XK_Meta_R)
      {
        altDown = down;
      }
      else if (sym == XK_Control_L || sym == XK_Control_R)
      {
        ctrlDown = down;
      }
      else if (sym == XK_Shift_L || sym == XK_Shift_R)
      {
        shiftDown = down;
      }
      else if (sym == XK_Super_L || sym == XK_Super_R)
      {
        superDown = down;
      }
      else
      {
        if (down && sym == XK_Escape && overlay.wheelOpen)
        {
          overlay.cancelGesture();
          return;
        }
        // Pressing any non-modifier key (e.g. Alt+Tab, Ctrl+C, Ctrl+V) disarms the overlay so shortcuts work normally
        if (down)
        {
          foreignKeyDown = true;
        }
        else
        {
          foreignKeyDown = false;
        }
      }
      if (!altDown && !ctrlDown && !shiftDown && !superDown)
      {
        foreignKeyDown = false;
      }
      updateArmedState();
    }

  private:
    void setupX11RawKeys()
    {
      dpy = XOpenDisplay(nullptr);
      if (!dpy)
        return;
      int eventBase = 0, errorBase = 0, major = 2, minor = 1;
      if (!XQueryExtension(dpy, "XInputExtension", &xiOpcode, &eventBase, &errorBase) ||
          XIQueryVersion(dpy, &major, &minor) != 0)
      {
        return;
      }
      unsigned char maskBits[XIMaskLen(XI_LASTEVENT)]{};
      XISetMask(maskBits, XI_RawKeyPress);
      XISetMask(maskBits, XI_RawKeyRelease);
      XIEventMask mask{XIAllMasterDevices, sizeof(maskBits), maskBits};
      XISelectEvents(dpy, DefaultRootWindow(dpy), &mask, 1);
      XFlush(dpy);

      int fd = ConnectionNumber(dpy);
      x11Notifier = std::make_unique<QSocketNotifier>(fd, QSocketNotifier::Read);
      QObject::connect(x11Notifier.get(), &QSocketNotifier::activated, this, [this]()
                       {
      while (XPending(dpy)) {
        XEvent ev;
        XNextEvent(dpy, &ev);
        if (ev.type == GenericEvent && ev.xcookie.extension == xiOpcode && XGetEventData(dpy, &ev.xcookie)) {
          if (ev.xcookie.evtype == XI_RawKeyPress || ev.xcookie.evtype == XI_RawKeyRelease) {
            const auto* raw = static_cast<XIRawEvent*>(ev.xcookie.data);
            KeySym sym = XkbKeycodeToKeysym(dpy, raw->detail, 0, 0);
            onKeySym(sym, ev.xcookie.evtype == XI_RawKeyPress);
          }
          XFreeEventData(dpy, &ev.xcookie);
        }
      } });
    }

    void setupEvdevKeys()
    {
      QDir devInput("/dev/input");
      for (const QString &name : devInput.entryList({"event*"}, QDir::System))
      {
        QString path = devInput.filePath(name);
        int fd = ::open(path.toLocal8Bit().constData(), O_RDONLY | O_NONBLOCK);
        if (fd < 0)
          continue;
        unsigned long evBits = 0;
        if (ioctl(fd, EVIOCGBIT(0, sizeof(evBits)), &evBits) < 0 || !(evBits & (1UL << EV_KEY)))
        {
          ::close(fd);
          continue;
        }
        evdevFds.push_back(fd);
        auto notifier = std::make_unique<QSocketNotifier>(fd, QSocketNotifier::Read);
        QObject::connect(notifier.get(), &QSocketNotifier::activated, this, [this, fd]()
                         {
        struct input_event ie;
        while (::read(fd, &ie, sizeof(ie)) == sizeof(ie)) {
          if (ie.type != EV_KEY || ie.value == 2) continue;  // ignore auto-repeat
          bool down = (ie.value == 1);
          switch (ie.code) {
            case KEY_LEFTALT:
            case KEY_RIGHTALT:
              onKeySym(XK_Alt_L, down);
              break;
            case KEY_LEFTCTRL:
            case KEY_RIGHTCTRL:
              onKeySym(XK_Control_L, down);
              break;
            case KEY_LEFTSHIFT:
            case KEY_RIGHTSHIFT:
              onKeySym(XK_Shift_L, down);
              break;
            case KEY_LEFTMETA:
            case KEY_RIGHTMETA:
              onKeySym(XK_Super_L, down);
              break;
            case KEY_ESC:
              onKeySym(XK_Escape, down);
              break;
            default:
              if (ie.code < 256) onKeySym(XK_a, down);
              break;
          }
        } });
        evdevNotifiers.push_back(std::move(notifier));
      }
    }
  };

  QString autostartDesktopPath()
  {
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    if (configDir.isEmpty())
      configDir = QDir::homePath() + "/.config";
    QDir().mkpath(configDir + "/autostart");
    return configDir + "/autostart/lolping-kde.desktop";
  }

  bool isAutostartEnabled()
  {
    return QFileInfo::exists(autostartDesktopPath());
  }

  void setAutostartEnabled(bool enabled)
  {
    QString path = autostartDesktopPath();
    if (!enabled)
    {
      QFile::remove(path);
      return;
    }
    QString exe = QCoreApplication::applicationFilePath();
    QString installedExe = QDir::homePath() + "/.local/bin/lolping-kde";
    if (QFileInfo::exists(installedExe))
      exe = installedExe;

    QFile f(path);
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
      QByteArray content =
          "[Desktop Entry]\n"
          "Type=Application\n"
          "Name=lolPing KDE Wayland\n"
          "Comment=League of Legends Ping & Emote Wheel for KDE Plasma Wayland\n"
          "Exec=\"" +
          exe.toUtf8() + "\"\n"
                         "Icon=lolping-kde\n"
                         "Terminal=false\n"
                         "Categories=Utility;Game;\n"
                         "X-KDE-autostart-after=panel\n";
      f.write(content);
    }
  }

  class SettingsDialog : public QDialog
  {
  public:
    AppSettings &settings;
    OverlayWindow &overlay;
    QCheckBox *chkAutostart = nullptr;
    QComboBox *cmbPingMod = nullptr;
    QComboBox *cmbEmoteMod = nullptr;
    QCheckBox *chkClickPing = nullptr;
    QCheckBox *chkClickEmote = nullptr;
    QSlider *sldVol = nullptr;
    QLabel *lblVol = nullptr;
    QSpinBox *spnWheel = nullptr;
    QSpinBox *spnPing = nullptr;
    QDoubleSpinBox *spnDuration = nullptr;
    std::array<QComboBox *, 8> cmbPingSlots{};
    QComboBox *cmbCenterPing = nullptr;
    std::array<QComboBox *, 8> cmbEmoteSlots{};
    QComboBox *cmbCenterEmote = nullptr;

    SettingsDialog(AppSettings &s, OverlayWindow &ov) : settings(s), overlay(ov)
    {
      setWindowTitle("lolPing — Settings");
      setMinimumSize(660, 480);
      setStyleSheet(R"(
      QDialog { background-color: #091428; color: #f0e6d2; }
      QTabWidget::pane { border: 1px solid #785a28; border-radius: 6px; background-color: #091428; top: -1px; }
      QTabBar::tab { background-color: #0a1c2e; color: #a09b8c; border: 1px solid #785a28; border-bottom: none; border-top-left-radius: 6px; border-top-right-radius: 6px; padding: 8px 20px; margin-right: 4px; font-weight: bold; }
      QTabBar::tab:selected { background-color: #1e282d; color: #f0e6d2; border-color: #c8aa6e; }
      QTabBar::tab:hover { color: #f0e6d2; }
      QGroupBox { border: 1px solid #785a28; border-radius: 6px; margin-top: 12px; padding-top: 10px; color: #c8aa6e; font-weight: bold; background-color: #0c1b2e; }
      QGroupBox#centerSlot { border: 1.5px solid #c8aa6e; background-color: #162436; color: #f0e6d2; }
      QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }
      QLabel { color: #f0e6d2; }
      QComboBox, QSpinBox, QDoubleSpinBox { background-color: #091428; color: #f0e6d2; border: 1px solid #785a28; border-radius: 4px; padding: 5px 8px; min-height: 24px; }
      QComboBox:hover, QSpinBox:hover, QDoubleSpinBox:hover { border-color: #c8aa6e; }
      QComboBox QAbstractItemView { background-color: #091428; color: #f0e6d2; selection-background-color: #1e282d; selection-color: #c8aa6e; border: 1px solid #c8aa6e; }
      QPushButton { background-color: #1e2328; color: #cdbe91; border: 1px solid #c8aa6e; border-radius: 4px; padding: 7px 14px; font-weight: bold; }
      QPushButton:hover { background-color: #2a323b; color: #f0e6d2; }
      QCheckBox { color: #f0e6d2; spacing: 8px; }
    )");

      auto *root = new QVBoxLayout(this);
      root->setContentsMargins(14, 14, 14, 14);
      root->setSpacing(12);

      auto *tabs = new QTabWidget();

      // ==================== TAB 1: GENERAL ====================
      auto *tabGeneral = new QWidget();
      auto *genLayout = new QVBoxLayout(tabGeneral);
      genLayout->setContentsMargins(12, 12, 12, 12);
      genLayout->setSpacing(10);

      auto *grpTrigger = new QGroupBox("Triggers & Behavior");
      auto *trigForm = new QFormLayout(grpTrigger);
      trigForm->setSpacing(10);

      cmbPingMod = new QComboBox();
      cmbPingMod->addItems({"Alt + Left Drag", "Ctrl + Left Drag", "Shift + Left Drag", "Super (Meta) + Left Drag"});
      QObject::connect(cmbPingMod, &QComboBox::currentIndexChanged, [this](int idx)
                       {
        if (idx >= 0) {
          settings.pingTrigger = static_cast<TriggerMod>(idx);
          settings.save();
        } });
      trigForm->addRow("Ping Wheel Trigger:", cmbPingMod);

      cmbEmoteMod = new QComboBox();
      cmbEmoteMod->addItems({"Alt + Left Drag", "Ctrl + Left Drag", "Shift + Left Drag", "Super (Meta) + Left Drag", "Disabled"});
      QObject::connect(cmbEmoteMod, &QComboBox::currentIndexChanged, [this](int idx)
                       {
        if (idx >= 0) {
          settings.emoteTrigger = static_cast<TriggerMod>(idx);
          settings.save();
        } });
      trigForm->addRow("Emote Wheel Trigger:", cmbEmoteMod);

      chkClickPing = new QCheckBox("Quick click (without dragging) places Center Ping");
      QObject::connect(chkClickPing, &QCheckBox::toggled, [this](bool on)
                       {
        settings.clickPing = on;
        settings.save(); });
      trigForm->addRow("Quick Click Ping:", chkClickPing);

      chkClickEmote = new QCheckBox("Quick click (without dragging) places Center Emote");
      QObject::connect(chkClickEmote, &QCheckBox::toggled, [this](bool on)
                       {
        settings.clickEmote = on;
        settings.save(); });
      trigForm->addRow("Quick Click Emote:", chkClickEmote);

      chkAutostart = new QCheckBox("Start lolPing automatically on login");
      QObject::connect(chkAutostart, &QCheckBox::toggled, [](bool on)
                       { setAutostartEnabled(on); });
      trigForm->addRow("System Startup:", chkAutostart);

      genLayout->addWidget(grpTrigger);

      auto *grpAppearance = new QGroupBox("Audio & Appearance");
      auto *appForm = new QFormLayout(grpAppearance);
      appForm->setSpacing(10);

      sldVol = new QSlider(Qt::Horizontal);
      sldVol->setRange(0, 100);
      lblVol = new QLabel();
      lblVol->setMinimumWidth(42);
      auto *volRow = new QHBoxLayout();
      volRow->addWidget(sldVol, 1);
      volRow->addWidget(lblVol);
      QObject::connect(sldVol, &QSlider::valueChanged, [this](int v)
                       {
        settings.volume = v;
        lblVol->setText(QString::number(v) + "%");
        settings.save(); });
      appForm->addRow("Sound Volume:", volRow);

      spnWheel = new QSpinBox();
      spnWheel->setRange(280, 720);
      spnWheel->setSingleStep(20);
      spnWheel->setSuffix(" px");
      QObject::connect(spnWheel, &QSpinBox::valueChanged, [this](int v)
                       {
        settings.wheelSize = v;
        settings.save(); });
      appForm->addRow("Wheel Diameter:", spnWheel);

      spnPing = new QSpinBox();
      spnPing->setRange(64, 220);
      spnPing->setSingleStep(10);
      spnPing->setSuffix(" px");
      QObject::connect(spnPing, &QSpinBox::valueChanged, [this](int v)
                       {
        settings.pingSize = v;
        settings.save(); });
      appForm->addRow("Ping / Emote Size:", spnPing);

      spnDuration = new QDoubleSpinBox();
      spnDuration->setRange(1.0, 6.0);
      spnDuration->setSingleStep(0.2);
      spnDuration->setDecimals(1);
      spnDuration->setSuffix(" s");
      QObject::connect(spnDuration, &QDoubleSpinBox::valueChanged, [this](double v)
                       {
        settings.pingDuration = v;
        settings.save(); });
      appForm->addRow("Display Duration:", spnDuration);

      genLayout->addWidget(grpAppearance);
      genLayout->addStretch(1);
      tabs->addTab(tabGeneral, "General");

      // ==================== TAB 2: PING WHEEL (3x3 SPATIAL GRID) ====================
      auto *tabPings = new QWidget();
      auto *pingsLayout = new QVBoxLayout(tabPings);
      pingsLayout->setContentsMargins(12, 12, 12, 12);
      pingsLayout->setSpacing(10);

      auto *lblPingHint = new QLabel("Arrange the 8 directional slices and the Center Quick-Click Ping matches their position on screen:");
      lblPingHint->setStyleSheet("color: #a09b8c;");
      pingsLayout->addWidget(lblPingHint);

      auto *pingGrid = new QGridLayout();
      pingGrid->setSpacing(10);

      struct GridPos
      {
        int slotIdx;
        int row;
        int col;
        const char *label;
      };
      constexpr GridPos kGridMap[8] = {
          {7, 0, 0, "Top-Left"},
          {0, 0, 1, "Top"},
          {1, 0, 2, "Top-Right"},
          {6, 1, 0, "Left"},
          {2, 1, 2, "Right"},
          {5, 2, 0, "Bottom-Left"},
          {4, 2, 1, "Bottom"},
          {3, 2, 2, "Bottom-Right"},
      };

      auto populatePingCombo = [this](QComboBox *cmb)
      {
        cmb->setIconSize(QSize(22, 22));
        for (const auto &pd : allPings())
        {
          cmb->addItem(QIcon(overlay.assets.texture(pd.icon)), pd.name, pd.id);
        }
      };

      for (const auto &gp : kGridMap)
      {
        auto *box = new QGroupBox(gp.label);
        auto *boxLay = new QVBoxLayout(box);
        auto *cmb = new QComboBox();
        populatePingCombo(cmb);
        cmbPingSlots[gp.slotIdx] = cmb;
        int idx = gp.slotIdx;
        QObject::connect(cmb, &QComboBox::currentIndexChanged, [this, cmb, idx](int)
                         {
          settings.pingWheel[idx] = cmb->currentData().toString();
          settings.save(); });
        boxLay->addWidget(cmb);
        pingGrid->addWidget(box, gp.row, gp.col);
      }

      {
        auto *centerBox = new QGroupBox("Center (Quick Click)");
        centerBox->setObjectName("centerSlot");
        auto *boxLay = new QVBoxLayout(centerBox);
        cmbCenterPing = new QComboBox();
        populatePingCombo(cmbCenterPing);
        QObject::connect(cmbCenterPing, &QComboBox::currentIndexChanged, [this](int)
                         {
          settings.clickPingId = cmbCenterPing->currentData().toString();
          settings.save(); });
        boxLay->addWidget(cmbCenterPing);
        pingGrid->addWidget(centerBox, 1, 1);
      }

      pingsLayout->addLayout(pingGrid);
      pingsLayout->addStretch(1);
      tabs->addTab(tabPings, "Ping Wheel");

      // ==================== TAB 3: EMOTE WHEEL (3x3 SPATIAL GRID) ====================
      auto *tabEmotes = new QWidget();
      auto *emotesLayout = new QVBoxLayout(tabEmotes);
      emotesLayout->setContentsMargins(12, 12, 12, 12);
      emotesLayout->setSpacing(10);

      auto *lblEmoteHint = new QLabel("Arrange the 8 directional emotes and the Center Quick-Click Emote:");
      lblEmoteHint->setStyleSheet("color: #a09b8c;");
      emotesLayout->addWidget(lblEmoteHint);

      auto *emoteGrid = new QGridLayout();
      emoteGrid->setSpacing(10);

      auto populateEmoteCombo = [this](QComboBox *cmb)
      {
        cmb->setIconSize(QSize(24, 24));
        for (const auto &ed : allEmotes())
        {
          cmb->addItem(QIcon(overlay.assets.emoteIcon(ed.slug)), ed.name, ed.slug);
        }
      };

      for (const auto &gp : kGridMap)
      {
        auto *box = new QGroupBox(gp.label);
        auto *boxLay = new QVBoxLayout(box);
        auto *cmb = new QComboBox();
        populateEmoteCombo(cmb);
        cmbEmoteSlots[gp.slotIdx] = cmb;
        int idx = gp.slotIdx;
        QObject::connect(cmb, &QComboBox::currentIndexChanged, [this, cmb, idx](int)
                         {
          settings.emoteWheel[idx] = cmb->currentData().toString();
          settings.save(); });
        boxLay->addWidget(cmb);
        emoteGrid->addWidget(box, gp.row, gp.col);
      }

      {
        auto *centerBox = new QGroupBox("Center (Quick Click)");
        centerBox->setObjectName("centerSlot");
        auto *boxLay = new QVBoxLayout(centerBox);
        cmbCenterEmote = new QComboBox();
        populateEmoteCombo(cmbCenterEmote);
        QObject::connect(cmbCenterEmote, &QComboBox::currentIndexChanged, [this](int)
                         {
          settings.clickEmoteSlug = cmbCenterEmote->currentData().toString();
          settings.save(); });
        boxLay->addWidget(cmbCenterEmote);
        emoteGrid->addWidget(centerBox, 1, 1);
      }

      emotesLayout->addLayout(emoteGrid);
      emotesLayout->addStretch(1);
      tabs->addTab(tabEmotes, "Emote Wheel");

      root->addWidget(tabs, 1);

      // ==================== BOTTOM ACTION BAR ====================
      auto *btnRow = new QHBoxLayout();
      auto *btnReset = new QPushButton("Reset Defaults");
      QObject::connect(btnReset, &QPushButton::clicked, [this]()
                       {
        settings = AppSettings{};
        settings.save();
        refreshUi(); });

      auto *btnTestPing = new QPushButton("Preview Ping");
      QObject::connect(btnTestPing, &QPushButton::clicked, [this]()
                       {
        QPointF c(overlay.width() * 0.5, overlay.height() * 0.45);
        overlay.spawnPingAt(settings.clickPingId, c); });

      auto *btnTestEmote = new QPushButton("Preview Emote");
      QObject::connect(btnTestEmote, &QPushButton::clicked, [this]()
                       {
        QPointF c(overlay.width() * 0.5, overlay.height() * 0.45);
        overlay.spawnEmoteAt(settings.clickEmoteSlug, c); });

      auto *btnClose = new QPushButton("Close");
      QObject::connect(btnClose, &QPushButton::clicked, this, &QDialog::hide);

      btnRow->addWidget(btnReset);
      btnRow->addWidget(btnTestPing);
      btnRow->addWidget(btnTestEmote);
      btnRow->addStretch(1);
      btnRow->addWidget(btnClose);
      root->addLayout(btnRow);

      refreshUi();
    }

    void refreshUi()
    {
      QSignalBlocker b1(cmbPingMod), b2(cmbEmoteMod), b3(chkClickPing), b4(chkClickEmote);
      QSignalBlocker b5(sldVol), b6(spnWheel), b7(spnPing), b8(spnDuration), b9(chkAutostart);

      cmbPingMod->setCurrentIndex(static_cast<int>(settings.pingTrigger));
      cmbEmoteMod->setCurrentIndex(static_cast<int>(settings.emoteTrigger));
      chkClickPing->setChecked(settings.clickPing);
      chkClickEmote->setChecked(settings.clickEmote);
      chkAutostart->setChecked(isAutostartEnabled());
      sldVol->setValue(settings.volume);
      lblVol->setText(QString::number(settings.volume) + "%");
      spnWheel->setValue(settings.wheelSize);
      spnPing->setValue(settings.pingSize);
      spnDuration->setValue(settings.pingDuration);

      for (int i = 0; i < 8; ++i)
      {
        QSignalBlocker bp(cmbPingSlots[i]);
        int pIdx = cmbPingSlots[i]->findData(settings.pingWheel[i]);
        cmbPingSlots[i]->setCurrentIndex(pIdx >= 0 ? pIdx : 0);

        QSignalBlocker be(cmbEmoteSlots[i]);
        int eIdx = cmbEmoteSlots[i]->findData(settings.emoteWheel[i]);
        cmbEmoteSlots[i]->setCurrentIndex(eIdx >= 0 ? eIdx : 0);
      }

      if (cmbCenterPing)
      {
        QSignalBlocker bc(cmbCenterPing);
        int cIdx = cmbCenterPing->findData(settings.clickPingId);
        cmbCenterPing->setCurrentIndex(cIdx >= 0 ? cIdx : 0);
      }
      if (cmbCenterEmote)
      {
        QSignalBlocker bc(cmbCenterEmote);
        int cIdx = cmbCenterEmote->findData(settings.clickEmoteSlug);
        cmbCenterEmote->setCurrentIndex(cIdx >= 0 ? cIdx : 0);
      }
    }
  };

  QString resolveAssetsDir()
  {
    QString appDir = QCoreApplication::applicationDirPath();
    QStringList candidates = {
        QDir::homePath() + "/.local/share/lolping-kde/assets",
        "/usr/share/lolping-kde/assets",
        appDir + "/../assets",
        appDir + "/assets",
        QDir::currentPath() + "/assets",
        "/home/iamhieu2012/Dev/lolPing/assets",
    };
    for (const QString &c : candidates)
    {
      if (QFileInfo::exists(c + "/textures/enemy_missing.png"))
      {
        return QFileInfo(c).canonicalFilePath();
      }
    }
    return "/home/iamhieu2012/Dev/lolPing/assets";
  }

} // namespace

int main(int argc, char *argv[])
{
  qputenv("QT_QPA_PLATFORM", "wayland");

  QSurfaceFormat fmt;
  fmt.setAlphaBufferSize(8);
  QSurfaceFormat::setDefaultFormat(fmt);

  QApplication app(argc, argv);
  app.setApplicationName("lolPing-KDE");
  app.setOrganizationName("lolPing");
  app.setQuitOnLastWindowClosed(false);

  ensureKWinWaylandSupport();

  AppSettings settings;
  settings.load();

  QString assetsPath = resolveAssetsDir();
  AssetStore assets(assetsPath);

  OverlayWindow overlay(settings, assets);
  overlay.show();

  GlobalKeyWatcher watcher(overlay, settings);
  SettingsDialog settingsDlg(settings, overlay);

  QSystemTrayIcon tray;
  QIcon trayIcon(assetsPath + "/textures/enemy_missing.png");
  tray.setIcon(trayIcon);
  tray.setToolTip("lolPing (Hold Alt + Left Drag)");

  QMenu trayMenu;
  QAction *actSettings = trayMenu.addAction("Settings...");
  QObject::connect(actSettings, &QAction::triggered, [&settingsDlg]()
                   {
    if (settingsDlg.chkAutostart) settingsDlg.chkAutostart->setChecked(isAutostartEnabled());
    settingsDlg.show();
    settingsDlg.raise();
    settingsDlg.activateWindow(); });

  QAction *actTest = trayMenu.addAction("Preview Ping");
  QObject::connect(actTest, &QAction::triggered, [&overlay]()
                   { overlay.spawnPingAt("missing", QPointF(overlay.width() * 0.5, overlay.height() * 0.5)); });

  QAction *actPause = trayMenu.addAction("Pause lolPing");
  actPause->setCheckable(true);
  actPause->setChecked(overlay.paused);
  QObject::connect(actPause, &QAction::toggled, [&overlay, actPause](bool)
                   {
    overlay.togglePause();
    actPause->setChecked(overlay.paused); });

  QAction *actAutostart = trayMenu.addAction("Start on Login");
  actAutostart->setCheckable(true);
  actAutostart->setChecked(isAutostartEnabled());
  QObject::connect(actAutostart, &QAction::toggled, [&settingsDlg](bool on)
                   {
    setAutostartEnabled(on);
    if (settingsDlg.chkAutostart) settingsDlg.chkAutostart->setChecked(on); });

  trayMenu.addSeparator();
  QAction *actQuit = trayMenu.addAction("Quit");
  QObject::connect(actQuit, &QAction::triggered, &app, &QApplication::quit);

  tray.setContextMenu(&trayMenu);
  QObject::connect(&tray, &QSystemTrayIcon::activated, [&settingsDlg](QSystemTrayIcon::ActivationReason reason)
                   {
    if (reason == QSystemTrayIcon::Trigger) {
      settingsDlg.setVisible(!settingsDlg.isVisible());
      if (settingsDlg.isVisible()) {
        settingsDlg.raise();
        settingsDlg.activateWindow();
      }
    } });
  tray.show();

  for (int i = 1; i < argc; ++i)
  {
    QString arg = QString::fromLocal8Bit(argv[i]);
    if (arg == "--settings")
    {
      settingsDlg.show();
    }
    else if (arg == "--demo")
    {
      QTimer::singleShot(400, [&overlay]()
                         { overlay.spawnPingAt("missing", QPointF(overlay.width() * 0.5, overlay.height() * 0.45)); });
    }
  }

  overlay.showToast("lolPing Ready", "Hold Alt + Left Drag for Pings | Ctrl + Left Drag for Emotes");
  return app.exec();
}
