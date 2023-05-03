/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgtkstyle_p.h"
#include "qgtkstyle_p_p.h"
#include "qcleanlooksstyle.h"

#include "qmaemo5style.h"
#include "qmaemo5style_p.h"
#include <private/qapplication_p.h>

#include <QtCore/QLibrary>
#include <QtWidgets/QStyleOption>
#include <QtWidgets/QStyleFactory>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QCommandLinkButton>
#include <QtWidgets/QAbstractScrollArea>
#include <QtWidgets/QAbstractItemView>
#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QScroller>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QAbstractSpinBox>
#include <QtX11Extras/QX11Info>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QCalendarWidget>
#include <QtCore/QTimer>
#include <QtCore/QTimeLine>
#include <QtGui/qpa/qplatformfontdatabase.h>

#include <QtDebug>

#include "qpixmapcache.h"
#undef signals // Collides with GTK stymbols
#include "qgtk2painter_p.h"
#include <private/qstylehelper_p.h>
#include <hildon/hildon.h>

QT_BEGIN_NAMESPACE

QMap<QAbstractScrollArea *, ScrollBarFader *> QMaemo5StylePrivate::scrollBarFaders;
int QMaemo5StylePrivate::scrollBarFadeDelay = 3000;         // [ms] hardcoded in hildon pannable area
int QMaemo5StylePrivate::scrollBarFadeDuration = 400;       // [ms] hardcoded in hildon pannable area
int QMaemo5StylePrivate::scrollBarFadeUpdateInterval = 100; // [ms] hardcoded in hildon pannable area
GtkWidget *QMaemo5StylePrivate::radioButtonLeft = 0;
GtkWidget *QMaemo5StylePrivate::radioButtonMiddle = 0;
GtkWidget *QMaemo5StylePrivate::radioButtonRight = 0;

//no longer in QGtkPainter, now static in qgtkstyle.cpp, so static here, too
static GtkStateType qt_gtk_state(const QStyleOption *option)
{
    GtkStateType state = GTK_STATE_NORMAL;
    if (!(option->state & QStyle::State_Enabled))
        state = GTK_STATE_INSENSITIVE;
    else if (option->state & QStyle::State_MouseOver)
        state = GTK_STATE_PRELIGHT;

    return state;
}

//used to be part of QStyleHelper
//requires qmath_p.h
#include <private/qmath_p.h>
static QPointF calcRadialPos(const QStyleOptionSlider *dial, qreal offset)
{
    const int width = dial->rect.width();
    const int height = dial->rect.height();
    const int r = qMin(width, height) / 2;
    const int currentSliderPosition = dial->upsideDown ? dial->sliderPosition : (dial->maximum - dial->sliderPosition);
    qreal a = 0;
    if (dial->maximum == dial->minimum)
        a = Q_PI / 2;
    else if (dial->dialWrapping)
        a = Q_PI * 3 / 2 - (currentSliderPosition - dial->minimum) * 2 * Q_PI
            / (dial->maximum - dial->minimum);
    else
        a = (Q_PI * 8 - (currentSliderPosition - dial->minimum) * 10 * Q_PI
            / (dial->maximum - dial->minimum)) / 6;
    qreal xc = width / 2.0;
    qreal yc = height / 2.0;
    qreal len = r - QStyleHelper::calcBigLineSize(r) - 3;
    qreal back = offset * len;
    QPointF pos(QPointF(xc + back * qCos(a), yc - back * qSin(a)));
    return pos;
}

QString QMaemo5StylePrivate::systemIconThemeName()
{
    QString qicontheme;
    gchar *gicontheme = 0;
    g_object_get(gtk_settings_get_default(), "gtk-icon-theme-name", &gicontheme, NULL);
    if (gicontheme)
        qicontheme = QString::fromUtf8(gicontheme);
    g_free(gicontheme);
    return qicontheme;
}

/*!
  \internal
  NOTE: Reverse color in fremantle themes are set only for
  GtkTextView widgets. GtkEntry returns a dark base color.
*/
GtkWidget* QMaemo5StylePrivate::getTextColorWidget() const
{
    return gtkWidget("GtkTextView");
}

/*!
    \internal
    \reimp
*/
void QMaemo5StylePrivate::initGtkMenu() const
{
    QGtkStylePrivate::initGtkMenu();

    // --- create widgets for the context sensitive menu
    GtkWidget *gtkMenu = gtk_menu_new();
    gtk_widget_set_name(gtkMenu, "hildon-context-sensitive-menu");
    gtk_widget_realize(gtkMenu);

    GtkWidget *gtkMenuItem = gtk_menu_item_new_with_label("X");
    gtk_menu_shell_append((GtkMenuShell*)gtkMenu, gtkMenuItem);
    gtk_widget_realize(gtkMenuItem);

    GtkWidget *gtkCheckMenuItem = gtk_check_menu_item_new_with_label("X");
    gtk_menu_shell_append((GtkMenuShell*)gtkMenu, gtkCheckMenuItem);
    gtk_widget_realize(gtkCheckMenuItem);

    GtkWidget *gtkMenuSeparator = gtk_separator_menu_item_new();
    gtk_menu_shell_append((GtkMenuShell*)gtkMenu, gtkMenuSeparator);

    addAllSubWidgets(gtkMenu);

    // --- and now for the "normal" app menu
    GtkWidget *gtkMenu2 = hildon_app_menu_new();

    // -- three menu buttons. Sapwood will use different background images depending on their position.
    radioButtonLeft = gtk_radio_button_new(NULL);
    gtk_widget_show(radioButtonLeft); // only a visible button is really added.
    hildon_app_menu_add_filter( (HildonAppMenu*)gtkMenu2, (GtkButton*)radioButtonLeft);

    radioButtonMiddle = gtk_radio_button_new(NULL);
    gtk_widget_show(radioButtonMiddle); // only a visible button is really added.
    hildon_app_menu_add_filter( (HildonAppMenu*)gtkMenu2, (GtkButton*)radioButtonMiddle);

    radioButtonRight = gtk_radio_button_new(NULL);
    gtk_widget_show(radioButtonRight); // only a visible button is really added.
    hildon_app_menu_add_filter( (HildonAppMenu*)gtkMenu2, (GtkButton*)radioButtonRight);

    addAllSubWidgets(gtkMenu2);

    // and now a hack for the sapwood engine
    radioButtonLeft->allocation.x = 0;
    radioButtonLeft->allocation.y = 0;
    GTK_WIDGET_FLAGS(radioButtonLeft) |= GTK_MAPPED;
    radioButtonMiddle->allocation.x = 1;
    radioButtonMiddle->allocation.y = 0;
    GTK_WIDGET_FLAGS(radioButtonMiddle) |= GTK_MAPPED;
    radioButtonRight->allocation.x = 2;
    radioButtonRight->allocation.y = 0;
    GTK_WIDGET_FLAGS(radioButtonRight) |= GTK_MAPPED;
}

void QMaemo5StylePrivate::applyCustomPaletteHash()
{
    QGtkStylePrivate::applyCustomPaletteHash();

    QPalette textpalette = gtkWidgetPalette("GtkTextView");
    qApp->setPalette(textpalette, "QAbstractSpinBox");
    qApp->setPalette(textpalette, "QLineEdit");
    qApp->setPalette(textpalette, "QTextEdit");
    qApp->setPalette(textpalette, "QPlainTextEdit");

    QPalette calendarpalette = QApplication::palette();
    QColor high = calendarpalette.color(QPalette::Highlight);
    QColor base = calendarpalette.color(QPalette::Window);
    calendarpalette.setColor(QPalette::Disabled, QPalette::Highlight, high.darker(130));
    calendarpalette.setColor(QPalette::AlternateBase, base.lighter(250));
    QApplication::setPalette(calendarpalette, "QCalendarWidget");

    QPalette treeviewpalette = gtkWidgetPalette("HildonPannableArea.GtkTreeView");
    GtkStyle *gtkStyle = gtk_widget_get_style(gtkWidget("HildonPannableArea.GtkTreeView"));

    GdkColor gdkBase = gtkStyle->base[GTK_STATE_NORMAL];
    GdkColor gdkText = gtkStyle->text[GTK_STATE_NORMAL];

    QColor text = QColor(gdkText.red>>8, gdkText.green>>8, gdkText.blue>>8);
    base = QColor(gdkBase.red>>8, gdkBase.green>>8, gdkBase.blue>>8);

    treeviewpalette.setBrush(QPalette::Base, base);
    treeviewpalette.setBrush(QPalette::Text, text);

    qApp->setPalette(treeviewpalette, "QScrollBar");
    qApp->setPalette(treeviewpalette, "QAbstractItemView");
    qApp->setPalette(treeviewpalette, "QScrollArea");
    qApp->setPalette(treeviewpalette, "QGraphicsView");

    if (gtkWidget("HildonNote-information-theme-portrait.GtkAlignment.GtkHBox.GtkVBox.GtkEventBox.GtkAlignment.GtkVBox.HildonNoteLabel-information-theme")) {
        qApp->setPalette(gtkWidgetPalette("HildonNote-information-theme-portrait.GtkAlignment.GtkHBox.GtkVBox.GtkEventBox.GtkAlignment.GtkVBox.HildonNoteLabel-information-theme"),
            "QMaemo5InformationBox");
    } else {
        qApp->setPalette(gtkWidgetPalette("HildonNote-information-theme.GtkAlignment.GtkHBox.GtkVBox.GtkEventBox.GtkAlignment.GtkVBox.HildonNoteLabel-information-theme"),
            "QMaemo5InformationBox");
    }
}
#define HILDON_WIDGET_ID "HILDON-WIDGET-"
const QLatin1String HILDON_MAGIC_NAME = QLS(HILDON_WIDGET_ID);

QString QMaemo5StylePrivate::getGtkNameFromQtName(const QWidget *widget)
{
    int index;
    if(widget && ((index = widget->objectName().indexOf(HILDON_MAGIC_NAME)) != -1))
        return widget->objectName().mid(index+sizeof(HILDON_WIDGET_ID)-1);
    else
        return QString();
}

GtkWidget * QMaemo5StylePrivate::getGtkButtonForWidget(const QWidget *widget)
{
    GtkWidget *gtkButton;
    QString gtkWidgetName = getGtkNameFromQtName(widget);
    if(gtkWidgetName.isEmpty())
        gtkButton = gtkWidget("HildonButton-finger");
    else if(!(gtkButton = gtkWidget(QHashableLatin1Literal::fromData(gtkWidgetName.toLatin1().constData()))))
        gtkButton = addGtkButtonWithName(gtkWidgetName);

    return gtkButton;
}

GtkWidget * QMaemo5StylePrivate::addGtkButtonWithName(const QString & widgetName)
{
    // Gtk bug - a GtkButton without a label won't even create it's child widgets
    GtkWidget *button = hildon_button_new(HILDON_SIZE_FINGER_HEIGHT, HILDON_BUTTON_ARRANGEMENT_VERTICAL);
    gtk_button_set_label(reinterpret_cast<GtkButton *>(button), "");
    if(!widgetName.isEmpty())
        gtk_widget_set_name(button, widgetName.toLatin1());
    addWidget(button);

    return button;
}

GtkWidget * QMaemo5StylePrivate::addGtkCheckButtonWithName(const QString & widgetName)
{
    GtkWidget *button = hildon_check_button_new(HILDON_SIZE_FINGER_HEIGHT);
    gtk_button_set_label(reinterpret_cast<GtkButton *>(button), "");
    if(!widgetName.isEmpty())
        gtk_widget_set_name(button, widgetName.toLatin1());
    addWidget(button);

    return button;
}

void QMaemo5StylePrivate::initGtkWidgets() const
{
    QGtkStylePrivate::initGtkWidgets();

    hildon_init();

    if (!gtkWidgetMap()->contains("GtkWindow"))
	return; // without window all other widgets won't be created cleanly

    // ComboBoxes buttons
    addWidget(hildon_number_editor_new(0,1));
    addWidget(hildon_entry_new(HILDON_SIZE_FINGER_HEIGHT));
    addWidget(gtk_text_view_new());
    addWidget(hildon_dialog_new_with_buttons("", NULL, GTK_DIALOG_MODAL, "", 0, NULL));
    addWidget(hildon_pannable_area_new());
    GtkWidget *note = hildon_note_new_information(NULL, "");
    gtk_widget_set_name(note, "HildonNote-InformationTheme");
    addWidget(note);

    // I need a tree view inside a pannable area
    GtkWidget *hildonPan = gtkWidget("HildonPannableArea");
    GtkWidget *gtkTreeView = gtk_tree_view_new();
    gtk_container_add((GtkContainer*)hildonPan, gtkTreeView);
    addWidget(gtkTreeView);

    addGtkButtonWithName(QString());
    addGtkCheckButtonWithName(QString());

    addWidget(hildon_edit_toolbar_new_with_text("", ""));

    // now for a special combobox trick...
    for (int i = 0; i < 3; i++) {
	const char *names[] = { "hildon-combobox-disabled-button", "hildon-combobox-active-button", "hildon-combobox-button" };

	GtkWidget *popup_button = hildon_button_new(HILDON_SIZE_FINGER_HEIGHT, HILDON_BUTTON_ARRANGEMENT_VERTICAL);
	gtk_widget_set_name(popup_button, names[i]);
	gtk_widget_set_size_request (popup_button, 60, -1);

	GtkWidget *address_entry = hildon_entry_new(HILDON_SIZE_FINGER_HEIGHT);
	GtkWidget *address_container_box = gtk_hbox_new(FALSE, 0);
	gtk_widget_set_name(address_container_box, "hildon-combobox");

	gtk_box_pack_start((GtkBox *)address_container_box, address_entry, TRUE, TRUE, 0);
	gtk_box_pack_end((GtkBox *)address_container_box, popup_button, FALSE, FALSE, 0);
	gtk_widget_show (address_entry);
	gtk_widget_show (popup_button);
	// gtk_widget_set_sensitive(popup_button, FALSE);

	addWidget(address_container_box);
    }
}

/*! \internal
 *  Returns one int value from the HildonAppMenu widget style.
 */
int QMaemo5StylePrivate::getAppMenuMetric( const char *metricName, int defaultValue ) const
{
    guint value = defaultValue;
    if (GtkWidget *hildonAppMenu = gtkWidget("HildonAppMenu"))
        gtk_widget_style_get(hildonAppMenu, metricName, &value, NULL);
    return value;
}

// Fetch the application font from the pango font description
// contained in the theme.
QFont QMaemo5StylePrivate::getThemeFont(const QString &logicalFontName)
{
    QFont font;
    GtkStyle *style;

    if (!logicalFontName.isEmpty())
	style = gtk_rc_get_style_by_paths(gtk_settings_get_default(), logicalFontName.toUtf8().constData(), NULL, G_TYPE_NONE);
    else
	style = gtkStyle();

    if (style && qApp->desktopSettingsAware())
    {
	PangoFontDescription *gtk_font = style->font_desc;
	font.setPointSizeF((float)(pango_font_description_get_size(gtk_font))/PANGO_SCALE);

	QString family = QString::fromLatin1(pango_font_description_get_family(gtk_font));
	if (!family.isEmpty())
	    font.setFamily(family);

#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
	const int weight = pango_font_description_get_weight(gtk_font);
	font.setWeight(QPlatformFontDatabase::weightFromInteger(weight));
#else
	int weight = pango_font_description_get_weight(gtk_font);
	if (weight >= PANGO_WEIGHT_HEAVY)
	    font.setWeight(QFont::Black);
	else if (weight >= PANGO_WEIGHT_BOLD)
	    font.setWeight(QFont::Bold);
	else if (weight >= PANGO_WEIGHT_SEMIBOLD)
	    font.setWeight(QFont::DemiBold);
	else if (weight >= PANGO_WEIGHT_NORMAL)
	    font.setWeight(QFont::Normal);
	else
	    font.setWeight(QFont::Light);
#endif

	PangoStyle fontstyle = pango_font_description_get_style(gtk_font);
	if (fontstyle == PANGO_STYLE_ITALIC)
	    font.setStyle(QFont::StyleItalic);
	else if (fontstyle == PANGO_STYLE_OBLIQUE)
	    font.setStyle(QFont::StyleOblique);
	else
	    font.setStyle(QFont::StyleNormal);
    }
    return font;
}

/*!
    \class QMaemo5Style
    \brief The QMaemo5Style class provides a widget style rendered by GTK+
    \since 4.6
    \ingroup qtmaemo5

    The QMaemo5Style style provides a look and feel that integrates Qt widgets
    into Hildon-based desktop environments.

    It is largely based on QGtkStyle, but does some special magic to "Hildonize" the look and feel.

    \note The style requires GTK+ version 2.10 or later and the special patched GTK+ Hildon include files.
*/

/*!
    Constructs a QMaemo5Style object.
*/
QMaemo5Style::QMaemo5Style()
    : QGtkStyle(*new QMaemo5StylePrivate)
{
}

/*!
    Destroys the QMaemo5Style object.
*/
QMaemo5Style::~QMaemo5Style()
{
}

/*!
    \reimp
    Sets the Maemo style file dialog hooks.
*/
void QMaemo5Style::polish(QApplication *app)
{
    Q_D(QMaemo5Style);

    QGtkStyle::polish(app);
}

ScrollBarFader::ScrollBarFader(QAbstractScrollArea *area, int delay, int duration, int update_interval)
    : QObject(area), m_area(area), m_current_alpha(0.0)
{
    m_fade_timeline = new QTimeLine(duration, this);
    m_fade_timeline->setUpdateInterval(update_interval);
    connect(m_fade_timeline, SIGNAL(valueChanged(qreal)), this, SLOT(fade(qreal)));
    m_delay_timer = new QTimer(this);
    m_delay_timer->setInterval(delay);
    connect(m_delay_timer, SIGNAL(timeout()), this, SLOT(delayTimeout()));
    area->installEventFilter(this);
    area->viewport()->installEventFilter(this);
}

ScrollBarFader::~ScrollBarFader()
{
    m_area->viewport()->removeEventFilter(this);
    m_area->removeEventFilter(this);
}


void ScrollBarFader::delayTimeout()
{
    m_fade_timeline->setDirection(QTimeLine::Backward);
    m_fade_timeline->resume();
    m_delay_timer->stop();
}

void ScrollBarFader::fade(qreal value)
{
    m_current_alpha = value;

    if (QScrollBar *hsb = m_area->horizontalScrollBar()) {
        if (hsb->isVisible()) {
            QStyleOptionSlider option;
            option.initFrom(hsb);
            option.subControls = QStyle::SC_All;
            hsb->update(); // ### hsb->style()->subControlRect(QStyle::CC_ScrollBar, &option, QStyle::SC_ScrollBarSlider, hsb));
        }
    }
    if (QScrollBar *vsb = m_area->verticalScrollBar()) {
        if (vsb->isVisible()) {
            QStyleOptionSlider option;
            option.initFrom(vsb);
            option.subControls = QStyle::SC_All;
            vsb->update(); // ### vsb->style()->subControlRect(QStyle::CC_ScrollBar, &option, QStyle::SC_ScrollBarSlider, vsb));
        }
    }
}


void ScrollBarFader::show()
{
    if (!m_delay_timer->isActive()) {
        m_fade_timeline->setDirection(QTimeLine::Forward);
        if (m_fade_timeline->state() != QTimeLine::Running)
            m_fade_timeline->start();
    }
    m_delay_timer->start();
}


bool ScrollBarFader::eventFilter(QObject *obj, QEvent *ev)
{
    if (m_area->isEnabled()) {
        if (m_area == obj) {
            switch (ev->type()) {
            case QEvent::Show:
            case QEvent::ScrollPrepare:
            case QEvent::Scroll: {
                show();
                break;
            }
            default:
                break;
            }
        } else if(ev->type() == QEvent::Wheel)
            show();
    }

    return QObject::eventFilter(obj, ev);
}

/*!
    \reimp
*/
void QMaemo5Style::polish(QWidget *widget)
{
    Q_D(QMaemo5Style);

    QGtkStyle::polish(widget);
    if (QAbstractScrollArea *area = qobject_cast<QAbstractScrollArea *>(widget)) {
        if ((qobject_cast<QTextEdit *>(widget) && !qobject_cast<QTextBrowser *>(widget)) || qobject_cast<QPlainTextEdit *>(widget)) {
            area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        } else {
            area->setFrameStyle(QFrame::NoFrame);

            d->scrollBarFaders.insert(area, new ScrollBarFader(area, d->scrollBarFadeDelay,
                                                                     d->scrollBarFadeDuration,
                                                                     d->scrollBarFadeUpdateInterval));

            if (QAbstractItemView *itemview = qobject_cast<QAbstractItemView *>(area)) {
                itemview->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
                itemview->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
            }

            QScroller::grabGesture(area, QScroller::LeftMouseButtonGesture);
        }
    } else if (qobject_cast<QCommandLinkButton*>(widget)) {
        widget->setFont(standardFont(QLS("SystemFont")));
        widget->setAttribute(Qt::WA_Hover, false);
    }
}

/*!
  \reimp
  */
void QMaemo5Style::unpolish(QWidget *widget)
{
    Q_D(QMaemo5Style);

    QGtkStyle::unpolish(widget);

    if (QAbstractScrollArea *area = qobject_cast<QAbstractScrollArea*>(widget)) {
        delete d->scrollBarFaders.take(area);

        if (QAbstractItemView *itemview = qobject_cast<QAbstractItemView *>(area))
            itemview->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
    }
}

/*!
    \reimp
*/
int QMaemo5Style::pixelMetric(QStyle::PixelMetric metric,
                           const QStyleOption *option,
                           const QWidget *widget) const
{
    Q_D(const QMaemo5Style);

    if (!d->isThemeAvailable())
    return QCleanlooksStyle().pixelMetric(metric, option, widget);

    switch (metric) {
    case PM_DefaultLayoutSpacing:
        return 8;
    case PM_DefaultTopLevelMargin:
        return 8;
    case PM_LayoutLeftMargin:
    case PM_LayoutRightMargin:
        return 16;
    case PM_LayoutTopMargin:
        return 0;
    case PM_LayoutBottomMargin:
        return 8;
    case PM_DialogButtonsButtonHeight:
        return 70;
    case PM_DialogButtonsButtonWidth:
        return 180;
    case PM_MessageBoxIconSize:
        return 0;
    case PM_ToolBarIconSize:
        return 48; // thumb icon size
    case PM_TabBarIconSize:
    case PM_ListViewIconSize:
        return 48; // thumb icon size
    case PM_TextCursorWidth:
        return 2;
    case PM_SplitterWidth:
        return 14;
    case PM_HeaderGripMargin:
        return 30;
    case PM_SmallIconSize:
        return 24;
    case PM_LargeIconSize:
        return 48;
    case PM_ButtonShiftHorizontal: {
        guint horizontal_shift = 1;
        if (GtkWidget *gtkButton = d->getGtkButtonForWidget(widget))
	    gtk_widget_style_get(gtkButton, "child-displacement-x", &horizontal_shift, NULL);
        return horizontal_shift;
    }
    case PM_ButtonShiftVertical: {
        guint vertical_shift = 1;
        if (GtkWidget *gtkButton = d->getGtkButtonForWidget(widget))
	    gtk_widget_style_get(gtkButton, "child-displacement-y", &vertical_shift, NULL);
        return vertical_shift;
    }
    case PM_ScrollBarSliderMin: // fall through
    case PM_ScrollBarExtent: {
        guint indicator_width = 6;
        if (GtkWidget *hildonPan = d->gtkWidget("HildonPannableArea"))
	    gtk_widget_style_get(hildonPan, "indicator-width", &indicator_width, NULL);
        return indicator_width;
    }
    case PM_ScrollView_ScrollBarSpacing:
        return 0;

    case PM_TabBarTabHSpace:
        return 38;
    case PM_TabBarTabVSpace:
        return 38;

    case PM_ExclusiveIndicatorWidth:
    case PM_ExclusiveIndicatorHeight:
    case PM_IndicatorWidth:
    case PM_IndicatorHeight: {
        gint size = 38, spacing = 8;
        if (GtkWidget *hildonCheckButton = d->gtkWidget("HildonCheckButton-finger"))
	    gtk_widget_style_get(hildonCheckButton, "image-spacing", &spacing, "checkbox-size", &size, NULL);
        return size + 2 * spacing;
    }
    case PM_CheckBoxLabelSpacing:
        return 3; // image-spacing is already calculated into PM_IndicatorWidth, but we're still 3pix to narrow...

    case PM_Maemo5AppMenuHorizontalSpacing:
        return d->getAppMenuMetric("horizontal-spacing", 16);
    case PM_Maemo5AppMenuVerticalSpacing:
        return d->getAppMenuMetric("vertical-spacing", 16);
    case PM_Maemo5AppMenuContentMargin:
        return d->getAppMenuMetric("inner-border", 16);
    case PM_Maemo5AppMenuLandscapeXOffset:
        return d->getAppMenuMetric("external-border", 50);
    case PM_Maemo5AppMenuFilterGroupWidth:
        return d->getAppMenuMetric("filter-group-width", 444);
    case PM_Maemo5AppMenuFilterGroupVerticalSpacing:
        return d->getAppMenuMetric("filter-vertical-spacing", 16);

    case PM_Maemo5EditBarBackButtonWidth: {
        gint width = 112;
        if (GtkWidget *hildonEditBar = d->gtkWidget("toolbar-edit-mode"))
	    gtk_widget_style_get(hildonEditBar, "arrow-width", &width, NULL);
        return width;
    }
    case PM_Maemo5EditBarBackButtonHeight: {
        gint height = 56;
        if (GtkWidget *hildonEditBar = d->gtkWidget("toolbar-edit-mode"))
	    gtk_widget_style_get(hildonEditBar, "arrow-height", &height, NULL);
        return height;
    }

    default:
        return  QGtkStyle::pixelMetric(metric, option, widget);
    }
}

/*!
    \reimp
*/
int QMaemo5Style::styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget,
                         QStyleHintReturn *returnData = 0) const
{
    Q_D(const QMaemo5Style);

    if (!d->isThemeAvailable())
        return QCleanlooksStyle().styleHint(hint, option, widget, returnData);

    switch (hint) {
    case SH_Menu_Scrollable:
        return int(true);
    case SH_DialogButtonBox_ButtonsHaveIcons:
    case SH_ScrollBar_ContextMenu:
        return int(false);
    case SH_UnderlineShortcut:
        return int(false);
    case SH_RequestSoftwareInputPanel:
        return int(RSIP_OnMouseClick);
    case SH_Slider_AbsoluteSetButtons:
        return int(Qt::LeftButton);
    case SH_Slider_PageSetButtons:
        return int(Qt::NoButton);
    case SH_ItemView_ActivateItemOnSingleClick:
        return int(true);
    case SH_ComboBox_ListMouseTracking:
        return int(false);
    case SH_ComboBox_Popup:
        return int(false);
    case SH_ComboBox_PopupFrameStyle:
        return int(QFrame::Plain | QFrame::Box);
    case SH_ProgressDialog_TextLabelAlignment:
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    default:
        return QGtkStyle::styleHint(hint, option, widget, returnData);
    }
}

/*!
    \reimp
*/
void QMaemo5Style::drawPrimitive(QStyle::PrimitiveElement element,
                              const QStyleOption *option,
                              QPainter *painter,
                              const QWidget *widget) const
{
    Q_D(const QMaemo5Style);

    if (!d->isThemeAvailable()) {
        QCleanlooksStyle().drawPrimitive(element, option, painter, widget);
        return;
    }

    QGtk2Painter gtkPainter;
    gtkPainter.reset(painter);

    switch (element) {
    case PE_PanelButtonTool: // fall through
    case PE_PanelButtonCommand: {
        bool isTool = (element == PE_PanelButtonTool);

        GtkStateType state = qt_gtk_state(option);
        GtkShadowType shadow = GTK_SHADOW_OUT;
        if (option->state & State_On || option->state & State_Sunken) {
            state = GTK_STATE_ACTIVE;
            shadow = GTK_SHADOW_IN;
        }
        GtkWidget *gtkButton = 0;
        if (isTool) {
            gtkButton = d->gtkWidget("GtkToolButton.GtkButton");
        } else if (widget && qobject_cast<QDialogButtonBox *>(widget->parentWidget())) {
            gtkButton = d->gtkWidget("HildonDialog.GtkAlignment.GtkHBox.hildon-dialog-action-area.GtkButton-finger");
            if (!gtkButton)
                // Maybe application was started in portrait mode so
                // there is an additional GtkVBox in the layout. Try it.
                gtkButton = d->gtkWidget("HildonDialog.GtkAlignment.GtkHBox.GtkVBox.hildon-dialog-action-area.GtkButton-finger");
        } else if (widget && widget->parentWidget() && !qstrcmp(widget->parentWidget()->metaObject()->className(), "QMaemo5EditBar")) {
            gtkButton = d->gtkWidget("toolbar-edit-mode.GtkAlignment.GtkHBox.GtkButton");
        } else {
            gtkButton = d->getGtkButtonForWidget(widget);
        }

        if (gtkButton) {
            gtkPainter.paintBox(gtkButton, "button", option->rect, state, shadow,
                                gtkButton->style, QString(), 70);
        }
        break;
    }
    case PE_IndicatorBranch:
        // Due to style guidelines, maemo5 does not draw
        // expanders. But for compatibilitys sake we have to
        if (option->state & State_Children) {
            if (option->state & State_Open)
                QCleanlooksStyle().drawPrimitive(PE_IndicatorArrowDown, option, painter, widget);
            else
                QCleanlooksStyle().drawPrimitive(PE_IndicatorArrowRight, option, painter, widget);
        }
        break;

    case PE_FrameFocusRect:
        // never required with Hildon
        break;

    case PE_Frame:
        if ((qobject_cast<const QTextEdit *>(widget) && !qobject_cast<const QTextBrowser *>(widget)) || qobject_cast<const QPlainTextEdit *>(widget))
            proxy()->drawPrimitive(PE_FrameLineEdit, option, painter, widget);
        else
            QGtkStyle::drawPrimitive(element, option, painter, widget);
        break;

    case PE_PanelLineEdit:
        {
            int lineWidth = 1;
            if (const QStyleOptionFrame *panel = qstyleoption_cast<const QStyleOptionFrame *>(option))
                lineWidth = panel->lineWidth;
            // For embedded line edits we rely on the parent widget to fill the
            // background. The entry_bg is not a uniform color in Hildon
            if (lineWidth > 0) {
                GtkWidget *gtkEntry = d->gtkWidget("HildonEntry-finger");
                uint resolve_mask = option->palette.resolve();
                QRect textRect = option->rect.adjusted(gtkEntry->style->xthickness, gtkEntry->style->ythickness,
                                                       -gtkEntry->style->xthickness, -gtkEntry->style->ythickness);
                if (widget && widget->testAttribute(Qt::WA_SetPalette) &&
                    resolve_mask & (1 << QPalette::Base)) // Palette overridden by user
                    painter->fillRect(textRect, option->palette.base());
                else {
                    gtkPainter.paintFlatBox(gtkEntry, "entry_bg", textRect,
                                            option->state & State_Enabled ? GTK_STATE_NORMAL : GTK_STATE_INSENSITIVE, GTK_SHADOW_NONE, gtkEntry->style);
                }
                proxy()->drawPrimitive(PE_FrameLineEdit, option, painter, widget);
            } else if (option->state & State_Enabled) {
                // Note this is a bit of a workaround. Open item editors require us to fill the line
                // edit background but doing this will make it impossible to get the correct
                // background color for disabled spin box, line edit and combo box.
                //
                // This would be more elegant if we could either get the correct fill color for
                // disabled line edits or if itemview did not draw the item when an editor is open
                painter->fillRect(option->rect.adjusted(lineWidth, lineWidth, -lineWidth, -lineWidth),
                                  option->palette.brush(QPalette::Base));
            }
        }
        break;

    case PE_FrameLineEdit: {
        GtkWidget *gtkEntry = d->gtkWidget("HildonEntry-finger");

        gboolean interior_focus;
        gint focus_line_width;
        gboolean state_hint = false;
        QRect rect = option->rect;
	gtk_widget_style_get(gtkEntry,
                               "interior-focus", &interior_focus,
                               "focus-line-width", &focus_line_width,
                               "state-hint", &state_hint, NULL);

        // See https://bugzilla.mozilla.org/show_bug.cgi?id=405421 for info about this hack
        g_object_set_data(G_OBJECT(gtkEntry), "transparent-bg-hint", GINT_TO_POINTER(TRUE));

        if (!interior_focus && option->state & State_HasFocus)
            rect.adjust(focus_line_width, focus_line_width, -focus_line_width, -focus_line_width);

        if (option->state & State_HasFocus)
            GTK_WIDGET_SET_FLAGS(gtkEntry, GTK_HAS_FOCUS);
        gtkPainter.paintShadow(gtkEntry, "entry", rect, option->state & State_Enabled ?
                               (state_hint && option->state & State_HasFocus ? GTK_STATE_ACTIVE : GTK_STATE_NORMAL) : GTK_STATE_INSENSITIVE,
                               GTK_SHADOW_IN, gtkEntry->style,
                               option->state & State_HasFocus ? QLS("focus") : QString());
        if (!interior_focus && option->state & State_HasFocus)
            gtkPainter.paintShadow(gtkEntry, "entry", option->rect, option->state & State_Enabled ?
                                   GTK_STATE_ACTIVE : GTK_STATE_INSENSITIVE,
                                   GTK_SHADOW_IN, gtkEntry->style, QLS("GtkEntryShadowIn"));

        if (option->state & State_HasFocus)
            GTK_WIDGET_UNSET_FLAGS(gtkEntry, GTK_HAS_FOCUS);
        break;
    }

    case PE_IndicatorCheckBox: {
        GtkShadowType shadow = GTK_SHADOW_OUT;
        GtkStateType state = qt_gtk_state(option);

        if (option->state & State_Sunken)
            state = GTK_STATE_ACTIVE;

        if (option->state & State_NoChange)
            shadow = GTK_SHADOW_ETCHED_IN;
        else if (option->state & State_On)
            shadow = GTK_SHADOW_IN;
        else
            shadow = GTK_SHADOW_OUT;

        int spacing;

        if (GtkWidget *gtkCheckButton = d->gtkWidget("HildonCheckButton-finger")) {
            GtkWidget *gtkCellView = d->gtkWidget("HildonCheckButton-finger.GtkAlignment.GtkHBox.GtkCellView");
            // Some styles such as aero-clone assume they can paint in the spacing area
            gtkPainter.setClipRect(option->rect);

	    gtk_widget_style_get(gtkCheckButton, "image-spacing", &spacing, NULL);

            QRect checkRect = option->rect.adjusted(spacing, spacing, -spacing, -spacing);

            gtkPainter.paintCheckbox(gtkCellView, checkRect, state, shadow, gtkCellView->style,
                                     QLS("checkbutton"));
        }
        break;
    }

    case PE_PanelItemViewRow:
        // This primitive is only used to draw selection behind selected expander arrows.
        // We try not to decorate the tree branch background unless you inherit from StyledItemDelegate
        // The reason for this is that a lot of code that relies on custom item delegates will look odd having
        // a gradient on the branch but a flat shaded color on the item itself.
        QCommonStyle::drawPrimitive(element, option, painter, widget);
        if (!option->state & State_Selected || option->rect.size().isEmpty()) {
            break;
        } else {
            if (const QAbstractItemView *view = qobject_cast<const QAbstractItemView*>(widget)) {
                if (!qobject_cast<QStyledItemDelegate*>(view->itemDelegate()))
                    break;
            }
        } // fall through
    case PE_PanelItemViewItem: {

            //To improve the performance we won't cache unusable states
            QStyle::State old_state = option->state;
            const_cast<QStyleOption*>(option)->state |= QStyle::State_Active;
            const_cast<QStyleOption*>(option)->state &= ~(QStyle::State_HasFocus | QStyle::State_MouseOver);

            BEGIN_STYLE_PIXMAPCACHE(QString::fromLatin1("panelitem"));
            QGtk2Painter gtkCachedPainter;
            gtkCachedPainter.reset(p);
            QRect cacheRect(0, 0, option->rect.width(), option->rect.height());
            if (GtkWidget *gtkTreeView = d->gtkWidget("HildonPannableArea.GtkTreeView")) {
                const char *detail = "cell_odd_ruled";
                const QStyleOptionViewItemV4 *vopt = qstyleoption_cast<const QStyleOptionViewItemV4 *>(option);
                if (vopt && vopt->features & QStyleOptionViewItemV2::Alternate)
                    detail = "cell_even_ruled";

                int rowHeight = 70;
                if (GtkWidget *gtkTreeView = d->gtkWidget("HildonPannableArea.GtkTreeView"))
		    gtk_widget_style_get(gtkTreeView, "row-height", &rowHeight, NULL);

                if (option->rect.height() != rowHeight) {
                    QPixmap scalePix(option->rect.width(), rowHeight);
                    scalePix.fill(Qt::transparent);
                    QPainter scalePainter(&scalePix);
            QGtk2Painter gtkScalePainter;
            gtkScalePainter.reset(&scalePainter);
                    gtkScalePainter.setUsePixmapCache(false); // cached externally

                    // the sapwood engine won't scale the image, but instead tile it, which looks ridiculous
                    gtkScalePainter.paintFlatBox(gtkTreeView, detail, QRect(0, 0, option->rect.width(), rowHeight),
                                                 option->state & State_Selected ? GTK_STATE_SELECTED :
                                                 option->state & State_Enabled ? GTK_STATE_NORMAL : GTK_STATE_INSENSITIVE,
                                                 GTK_SHADOW_NONE, gtkTreeView->style);

                    // don't just scale the whole pixmap - the bottom border line would look extremly ugly for big items
                    int dh = 8; // just an arbitrary value which looks good with the default Maemo styles
                    p->drawPixmap(cacheRect.topLeft(), scalePix, QRect(0, 0, scalePix.width(), dh));
                    p->drawPixmap(cacheRect.adjusted(0, dh, 0, -dh), scalePix, QRect(0, dh, scalePix.width(), scalePix.height() - 2 * dh));
                    p->drawPixmap(cacheRect.bottomLeft() - QPoint(0, dh), scalePix, QRect(0, scalePix.height() - dh, scalePix.width(), dh));
                } else {
                    gtkCachedPainter.paintFlatBox(gtkTreeView, detail, cacheRect,
                                            option->state & State_Selected ? GTK_STATE_SELECTED :
                                            option->state & State_Enabled ? GTK_STATE_NORMAL : GTK_STATE_INSENSITIVE,
                                            GTK_SHADOW_NONE, gtkTreeView->style);
                }
            }
            END_STYLE_PIXMAPCACHE
            const_cast<QStyleOption*>(option)->state = old_state;
        break;
    }
    case PE_Maemo5InformationBox: {

        GtkWidget *hildonInformation;
        hildonInformation = d->gtkWidget("HildonNote-information-theme");
        if(!hildonInformation)
            hildonInformation = d->gtkWidget("HildonNote-information-theme-portrait");
        if (hildonInformation)
            gtkPainter.paintFlatBox(hildonInformation, NULL, option->rect, GTK_STATE_NORMAL, GTK_SHADOW_NONE, hildonInformation->style, QString());
        break;
    }
    case PE_Maemo5AppMenu: {
        if (GtkWidget *hildonAppMenu = d->gtkWidget("HildonAppMenu"))
            gtkPainter.paintFlatBox(hildonAppMenu, NULL, option->rect, GTK_STATE_NORMAL, GTK_SHADOW_NONE, hildonAppMenu->style, QString());
        break;
    }
    case PE_Maemo5EditBar: {
        if (GtkWidget *hildonEditBar = d->gtkWidget("toolbar-edit-mode"))
            gtkPainter.paintFlatBox(hildonEditBar, NULL, option->rect, GTK_STATE_NORMAL, GTK_SHADOW_NONE, hildonEditBar->style, QString());
        break;
    }
    case PE_Maemo5EditBarBackButton: {
        if (GtkWidget *hildonEditBarBackButton = d->gtkWidget("toolbar-edit-mode.hildon-edit-toolbar-arrow")) {
            GtkStateType state = qt_gtk_state(option);
            GtkShadowType shadow = GTK_SHADOW_OUT;
            if (option->state & State_On || option->state & State_Sunken) {
                state = GTK_STATE_ACTIVE;
                shadow = GTK_SHADOW_IN;
            }
            gtkPainter.paintBox(hildonEditBarBackButton, "button", option->rect, state, shadow, hildonEditBarBackButton->style, QString());
        }
        break;
    }
    default:
        QGtkStyle::drawPrimitive(element, option, painter, widget);
    }
}

/*!
    \reimp
*/
void QMaemo5Style::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                                   QPainter *painter, const QWidget *widget) const
{
    Q_D(const QMaemo5Style);

    if (!d->isThemeAvailable()) {
        QCleanlooksStyle().drawComplexControl(control, option, painter, widget);
        return;
    }

    QGtk2Painter gtkPainter;
    gtkPainter.reset(painter);

    switch (control) {
    case CC_Dial:
        if (const QStyleOptionSlider *scrollBar = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            QPalette pal = option->palette;
            QColor buttonColor(0x404040); //= pal.button().color();
            const int width = option->rect.width();
            const int height = option->rect.height();
            const bool enabled = option->state & QStyle::State_Enabled;
            qreal r = qMin(width, height) / 2;
            r -= r/50;
            const qreal penSize = r/20.0;

            painter->save();
            painter->setRenderHint(QPainter::Antialiasing);

            // Draw notches
            if (option->subControls & QStyle::SC_DialTickmarks) {
                painter->setPen(option->palette.dark().color().darker(120));
                painter->drawLines(QStyleHelper::calcLines(scrollBar));
            }

            // Cache dial background
            BEGIN_STYLE_PIXMAPCACHE(QString::fromLatin1("qdial"));
            p->setRenderHint(QPainter::Antialiasing);

            const qreal d_ = r / 6;
            const qreal dx = option->rect.x() + d_ + (width - 2 * r) / 2 + 1;
            const qreal dy = option->rect.y() + d_ + (height - 2 * r) / 2 + 1;

            QRectF br = QRectF(dx + 0.5, dy + 0.5,
                               int(r * 2 - 2 * d_ - 2),
                               int(r * 2 - 2 * d_ - 2));
            QColor shadowColor(0, 0, 0, 20);

            if (enabled) {
                // Drop shadow
                qreal shadowSize = qMax(1.0, penSize/2.0);
                QRectF shadowRect= br.adjusted(-2*shadowSize, -2*shadowSize,
                                               2*shadowSize, 2*shadowSize);
                QRadialGradient shadowGradient(shadowRect.center().x(),
                                               shadowRect.center().y(), shadowRect.width()/2.0,
                                               shadowRect.center().x(), shadowRect.center().y());
                shadowGradient.setColorAt(qreal(0.91), QColor(0, 0, 0, 40));
                shadowGradient.setColorAt(qreal(1.0), Qt::transparent);
                p->setBrush(shadowGradient);
                p->setPen(Qt::NoPen);
                p->translate(shadowSize, shadowSize);
                p->drawEllipse(shadowRect);
                p->translate(-shadowSize, -shadowSize);

                // Main gradient
                QRadialGradient gradient(br.center().x() - br.width()/3, dy,
                                         br.width()*1.3, br.center().x(),
                                         br.center().y() - br.height()/2);
                gradient.setColorAt(0, buttonColor);
                gradient.setColorAt(qreal(0.6), buttonColor);
                gradient.setColorAt(qreal(0.601), buttonColor.darker(107));
                gradient.setColorAt(1, buttonColor.darker(107));
                p->setBrush(gradient);
            } else {
                p->setBrush(Qt::NoBrush);
            }

            p->setPen(QPen(buttonColor.lighter(160), 2));
            p->drawEllipse(br);
            p->setBrush(Qt::NoBrush);
            p->setPen(buttonColor.lighter(110));
            p->drawEllipse(br.adjusted(1, 1, -1, -1));

            if (option->state & QStyle::State_HasFocus) {
                QColor highlight = pal.highlight().color();
                highlight.setHsv(highlight.hue(),
                                 qMin(190, highlight.saturation()),
                                 qMax(210, highlight.value()));
                highlight.setAlpha(200);
                p->setPen(QPen(highlight, 2.0));
                p->setBrush(Qt::NoBrush);
                p->drawEllipse(br.adjusted(-1, -1, 1, 1));
            }

            END_STYLE_PIXMAPCACHE

            QPointF dp = calcRadialPos(scrollBar, qreal(0.70));
            buttonColor = buttonColor.darker(104);
            buttonColor.setAlphaF(qreal(0.8));
            const qreal ds = r/qreal(7.0);
            QRectF dialRect(dp.x() - ds, dp.y() - ds, 2*ds, 2*ds);
            QRadialGradient dialGradient(dialRect.center().x() + dialRect.width()/2,
                                         dialRect.center().y() + dialRect.width(),
                                         dialRect.width()*2,
                                         dialRect.center().x(), dialRect.center().y());
            dialGradient.setColorAt(1, buttonColor.darker(130));
            dialGradient.setColorAt(qreal(0.4), buttonColor.darker(160));
            dialGradient.setColorAt(0, buttonColor.darker(130));
            if (penSize > 3.0) {
                painter->setPen(QPen(QColor(255,255,255,150), penSize));
                painter->drawLine(calcRadialPos(scrollBar, qreal(0.90)), calcRadialPos(scrollBar, qreal(0.96)));
            }
            painter->setBrush(dialGradient);
            painter->setPen(QPen(Qt::white, 3));
            painter->drawEllipse(dialRect.adjusted(-1, -1, 1, 1));
            painter->setPen(QPen(Qt::black, 2));
            painter->drawEllipse(dialRect);
            painter->restore();
    }
    break;
    case CC_ScrollBar:
        if (const QStyleOptionSlider *scrollBar = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            painter->fillRect(option->rect, option->palette.background());
            QRect scrollBarSlider = proxy()->subControlRect(control, scrollBar, SC_ScrollBarSlider, widget);
            QColor color = standardColor(QLS("SecondaryTextColor"));

            if (widget && widget->parentWidget()) {
                if (QAbstractScrollArea *area = qobject_cast<QAbstractScrollArea*>(widget->parentWidget()->parentWidget())) {
                    if (ScrollBarFader *fader = d->scrollBarFaders.value(area)) {
                        color.setAlphaF(fader->currentAlpha());
                    }
                }
            }

            painter->fillRect(scrollBarSlider, color);
        }
        break;
    case CC_SpinBox:
        if (const QStyleOptionSpinBox *spinBox = qstyleoption_cast<const QStyleOptionSpinBox *>(option)) {
            bool sunken = (spinBox->state & State_Sunken);
            bool upIsActive = (spinBox->activeSubControls == SC_SpinBoxUp);
            bool downIsActive = (spinBox->activeSubControls == SC_SpinBoxDown);
            bool reverse = (spinBox->direction == Qt::RightToLeft);

            QStyleOption editOpt = *spinBox;
            QRect editRect = subControlRect(CC_SpinBox, option, SC_SpinBoxFrame, widget);
            editOpt.rect = editRect;
            drawPrimitive(PE_PanelLineEdit, &editOpt, painter, widget);

            if (spinBox->buttonSymbols != QAbstractSpinBox::NoButtons) {
                QRect plusRect = subControlRect(CC_SpinBox, option, SC_SpinBoxUp, widget);
                QRect minusRect = subControlRect(CC_SpinBox, option, SC_SpinBoxDown, widget);
                QColor textColor = option->palette.color(QPalette::Text);
                QColor highlightedTextColor = option->palette.color(QPalette::Highlight);
                QColor disabledColor = option->palette.color(QPalette::Disabled, QPalette::Text);
                disabledColor.setAlpha(100);
                int size = 10;
                int gripMargin = 1;
                int centerX = minusRect.center().x();
                int centerY = minusRect.center().y();

                // minus
                centerX = minusRect.center().x();
                QColor penColor = (downIsActive && sunken) ? highlightedTextColor : textColor;
                painter->setPen(QPen(penColor, 5));
                painter->drawLine(centerX - size, centerY, centerX + size, centerY);

                // plus
                centerX = plusRect.center().x();
                penColor = (upIsActive && sunken) ? highlightedTextColor : textColor;
                painter->setPen(QPen(penColor, 5));
                painter->drawLine(centerX - size, centerY, centerX + size, centerY);
                painter->drawLine(centerX, centerY - size, centerX, centerY + size);

                // dots
                size = 15;
                textColor.setAlpha(50);
                QRect scrollBarSlider = option->rect;
                QRect rightRect = reverse ? minusRect : plusRect;
                QRect leftRect = reverse ? plusRect: minusRect;
                for (int i = -size; i < size ; i += 4) {
                    QPoint left(leftRect.right() + gripMargin, leftRect.center().y()+ i);
                    painter->fillRect(QRect(left, QSize(2,2)), textColor);
                    QPoint right(rightRect.left() + gripMargin - 1, rightRect.center().y() + i);
                    painter->fillRect(QRect(right, QSize(2,2)), textColor);
                }
            }
        }

        break;

    case CC_ComboBox:
        if (const QStyleOptionComboBox *combo = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
            if (!combo->editable) {
                QStyleOptionButton button;
                button.QStyleOption::operator=(*option); // copy QStyleOption members
                button.features = QStyleOptionButton::None;
                button.icon = combo->currentIcon;
                button.iconSize = combo->iconSize;
                button.text = QString();

                proxy()->drawControl(QStyle::CE_PushButtonBevel, &button, painter, widget);
            } else {
                if (const QStyleOptionComboBox *comboBox = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
                    bool sunken = comboBox->state & State_On; // play dead, if combobox has no items
                    BEGIN_STYLE_PIXMAPCACHE(QString::fromLatin1("cb-%0-%1").arg(sunken).arg(comboBox->editable));
                    QGtk2Painter gtkCachedPainter;
                    gtkCachedPainter.reset(p);

                    gtkCachedPainter.setUsePixmapCache(false); // cached externally

                    bool isEnabled = (comboBox->state & State_Enabled);
                    bool focus = isEnabled && (comboBox->state & State_HasFocus);
                    QColor buttonShadow = option->palette.dark().color();
                    GtkStateType state = qt_gtk_state(option);
                    int appears_as_list = !proxy()->styleHint(QStyle::SH_ComboBox_Popup, comboBox, widget);
                    QPixmap cache;
                    QString pixmapName;
                    QStyleOptionComboBox comboBoxCopy = *comboBox;
                    comboBoxCopy.rect = option->rect;

                    bool reverse = (option->direction == Qt::RightToLeft);
                    QRect rect = option->rect;
                    QRect arrowButtonRect = proxy()->subControlRect(CC_ComboBox, &comboBoxCopy,
                                                           SC_ComboBoxArrow, widget);
                    QRect editRect = proxy()->subControlRect(CC_ComboBox, &comboBoxCopy,
                                                    SC_ComboBoxEditField, widget);

                    GtkShadowType shadow = (option->state & State_Sunken || option->state & State_On ) ?
                                           GTK_SHADOW_IN : GTK_SHADOW_OUT;

                    // We use the gtk widget to position arrows and separators for us
                    GtkWidget *gtkCombo = d->gtkWidget("hildon-combobox");
                    GtkAllocation geometry = {0, 0, option->rect.width(), option->rect.height()};
		    gtk_widget_set_direction(gtkCombo, reverse ? GTK_TEXT_DIR_RTL : GTK_TEXT_DIR_LTR);
		    gtk_widget_size_allocate(gtkCombo, &geometry);

                    QHashableLatin1Literal buttonPath("hildon-combobox.hildon-combobox-disabled-button");
                    if (focus)
                        buttonPath = "hildon-combobox.hildon-combobox-active-button";
                    else if (isEnabled)
                        buttonPath = "hildon-combobox.hildon-combobox-button";
                    GtkWidget *gtkToggleButton = d->gtkWidget(buttonPath);
		    gtk_widget_set_direction(gtkToggleButton, reverse ? GTK_TEXT_DIR_RTL : GTK_TEXT_DIR_LTR);
                    if (gtkToggleButton && (appears_as_list || comboBox->editable)) {
                        if (focus)
                            GTK_WIDGET_SET_FLAGS(gtkToggleButton, GTK_HAS_FOCUS);
                        // Draw the combo box as a line edit with a button next to it
                        if (comboBox->editable || appears_as_list) {
                            GtkStateType frameState = (state == GTK_STATE_PRELIGHT) ? GTK_STATE_NORMAL : state;
                            if (focus)
                                frameState = GTK_STATE_ACTIVE;
                            QHashableLatin1Literal entryPath("hildon-combobox.HildonEntry-finger");
                            GtkWidget *gtkEntry = d->gtkWidget(entryPath);
			    gtk_widget_set_direction(gtkEntry, reverse ? GTK_TEXT_DIR_RTL : GTK_TEXT_DIR_LTR);
                            QRect frameRect = option->rect;

                            if (reverse)
                                frameRect.setLeft(arrowButtonRect.right());
                            else
                                frameRect.setRight(arrowButtonRect.left());

                            // Fill the line edit background
                            // We could have used flat_box with "entry_bg" but that is probably not worth the overhead
                            uint resolve_mask = option->palette.resolve();
                            int xt = gtkEntry->style->xthickness;
                            int yt = gtkEntry->style->ythickness;
                            QRect contentRect = frameRect.adjusted(xt, yt, -xt, -yt);
                            // Required for inner blue highlight with clearlooks
                            if (focus)
                                GTK_WIDGET_SET_FLAGS(gtkEntry, GTK_HAS_FOCUS);

                            if (widget && widget->testAttribute(Qt::WA_SetPalette) &&
                                resolve_mask & (1 << QPalette::Base)) // Palette overridden by user
                                painter->fillRect(contentRect, option->palette.base().color());
                            else {
                                gtkCachedPainter.paintFlatBox(gtkEntry, "entry_bg", contentRect,
                                                        option->state & State_Enabled ? GTK_STATE_NORMAL : GTK_STATE_INSENSITIVE,
                                                        GTK_SHADOW_NONE, gtkEntry->style, entryPath.toString() + QString::number(focus));
                            }
                            gtkCachedPainter.paintShadow(gtkEntry, comboBox->editable ? "entry" : "frame", frameRect, frameState,
                                                   GTK_SHADOW_IN, gtkEntry->style, entryPath.toString() +
                                                   QString::number(focus) + QString::number(comboBox->editable) +
                                                   QString::number(option->direction)/* + QString::number(frameState)*/);
                            if (focus)
                                GTK_WIDGET_UNSET_FLAGS(gtkEntry, GTK_HAS_FOCUS);
                        }

                        GtkStateType buttonState = GTK_STATE_NORMAL;

                        if (!(option->state & State_Enabled))
                            buttonState = GTK_STATE_INSENSITIVE;
                        else if (option->state & State_Sunken || option->state & State_On)
                            buttonState = GTK_STATE_ACTIVE;
                        else if (option->state & State_MouseOver && comboBox->activeSubControls & SC_ComboBoxArrow)
                            buttonState = GTK_STATE_PRELIGHT;

                        Q_ASSERT(gtkToggleButton);
                        gtkCachedPainter.paintBox( gtkToggleButton, "button", arrowButtonRect, buttonState,
                                             shadow, gtkToggleButton->style, buttonPath.toString() +
                                             QString::number(focus) + QString::number(option->direction));
                        if (focus)
                            GTK_WIDGET_UNSET_FLAGS(gtkToggleButton, GTK_HAS_FOCUS);
                    }

                    END_STYLE_PIXMAPCACHE;
                }
            }
        }
        break;

    default:
        QGtkStyle::drawComplexControl(control, option, painter, widget);
    }
}

static QHBoxLayout *findHBoxLayoutContaining(const QWidget *widget, QLayout *lay)
{
    for (int i = 0; i < lay->count(); ++i) {
        if (QLayout *sublay = lay->itemAt(i)->layout()) {
            if (QHBoxLayout *box = findHBoxLayoutContaining(widget, sublay))
                return box;
        } else if (lay->itemAt(i)->widget() == widget) {
            return qobject_cast<QHBoxLayout *>(lay);
        }
    }
    return 0;
}

/*!
    \reimp
*/
void QMaemo5Style::drawControl(ControlElement element,
                               const QStyleOption *option,
                               QPainter *painter,
                               const QWidget *widget) const
{
    Q_D(const QMaemo5Style);

    if (!d->isThemeAvailable()) {
        QCleanlooksStyle().drawControl(element, option, painter, widget);
        return;
    }

    QGtk2Painter gtkPainter;
    gtkPainter.reset(painter);

    switch (element) {
    case CE_PushButtonBevel:
        // This code is copied from commonstyle sans the CommandLink part
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            QRect br = btn->rect;
            int dbi = proxy()->pixelMetric(PM_ButtonDefaultIndicator, btn, widget);
            if (btn->features & QStyleOptionButton::DefaultButton)
                proxy()->drawPrimitive(PE_FrameDefaultButton, option, painter, widget);
            if (btn->features & QStyleOptionButton::AutoDefaultButton)
                br.setCoords(br.left() + dbi, br.top() + dbi, br.right() - dbi, br.bottom() - dbi);
            if (!(btn->features & (QStyleOptionButton::Flat))
                || btn->state & (State_Sunken | State_On)
                || (btn->features & QStyleOptionButton::CommandLinkButton && btn->state & State_MouseOver)) {
                QStyleOptionButton tmpBtn = *btn;
                tmpBtn.rect = br;
                proxy()->drawPrimitive(PE_PanelButtonCommand, &tmpBtn, painter, widget);
            }
            if (btn->features & QStyleOptionButton::HasMenu) {
                int mbi = proxy()->pixelMetric(PM_MenuButtonIndicator, btn, widget);
                QRect ir = btn->rect;
                QStyleOptionButton newBtn = *btn;
                newBtn.rect = QRect(ir.right() - mbi + 2, ir.height()/2 - mbi/2 + 3, mbi - 6, mbi - 6);
                proxy()->drawPrimitive(PE_IndicatorArrowDown, &newBtn, painter, widget);
            }
        }
        break;
    case CE_RadioButton:
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            bool handled = false;

            if (qobject_cast<const QRadioButton *>(widget) && widget->parentWidget() && widget->parentWidget()->layout()) {
                if (QHBoxLayout *box = findHBoxLayoutContaining(widget, widget->parentWidget()->layout())) {
                    QList<const QRadioButton *> buttonList;
                    for (int i = 0; i < box->count(); ++i) {
                        if (QRadioButton *radio = qobject_cast<QRadioButton *>(box->itemAt(i)->widget()))
                            buttonList << radio;
                    }

                    GtkWidget *gtkButton = 0;
                    int pos = buttonList.indexOf(qobject_cast<const QRadioButton *>(widget));
                    if (pos == 0)
                        gtkButton = d->radioButtonLeft;
                    else if (pos == buttonList.count() - 1)
                        gtkButton = d->radioButtonRight;
                    else
                        gtkButton = d->radioButtonMiddle;

                    GtkStateType state = qt_gtk_state(option);
                    GtkShadowType shadow = GTK_SHADOW_OUT;
                    if (option->state & State_On || option->state & State_Sunken) {
                        state = GTK_STATE_ACTIVE;
                        shadow = GTK_SHADOW_IN;
                    }
                    if (gtkButton) {
                        gtkPainter.paintBox(gtkButton, "button", option->rect, state, shadow,
                                            gtkButton->style, QString(), 70);
                    }
                    handled = true;
                }
            }
            if (!handled)
                proxy()->drawControl(CE_PushButtonBevel, btn, painter, widget);

            QStyleOptionButton subopt = *btn;
            subopt.rect = proxy()->subElementRect(SE_RadioButtonContents, btn, widget);
            proxy()->drawControl(CE_RadioButtonLabel, &subopt, painter, widget);
        }
        break;

    case CE_CheckBox:
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            QStyleOptionButton bevelopt = *btn;
            bevelopt.state &= ~State_On;
            proxy()->drawControl(CE_PushButtonBevel, &bevelopt, painter, widget);
            QStyleOptionButton subopt = *btn;
            subopt.rect = subElementRect(SE_PushButtonContents, btn, widget);
            QStyleOptionButton checkopt = subopt;
            checkopt.rect = proxy()->subElementRect(SE_CheckBoxIndicator, &subopt, widget);
            proxy()->drawPrimitive(PE_IndicatorCheckBox, &checkopt, painter, widget);
            checkopt.rect = subElementRect(SE_CheckBoxContents, &subopt, widget);
            proxy()->drawControl(CE_CheckBoxLabel, &checkopt, painter, widget);
        }
        break;

    case CE_PushButton:
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            proxy()->drawControl(CE_PushButtonBevel, btn, painter, widget);
            QStyleOptionButton subopt = *btn;
            subopt.rect = subElementRect(SE_PushButtonContents, btn, widget);
            proxy()->drawControl(CE_PushButtonLabel, &subopt, painter, widget);
        }
        break;

    case CE_RadioButtonLabel: // fall through
    case CE_PushButtonLabel:
        if (const QStyleOptionMaemo5ValueButton *button = qstyleoption_cast<const QStyleOptionMaemo5ValueButton *>(option)) {
            QRect ir = button->rect;
            uint tf = Qt::AlignVCenter | Qt::TextHideMnemonic | Qt::AlignLeft;
            int spacing = 8;
            QRect textRect, valueRect, iconRect;

            if (!button->text.isEmpty())
                textRect = ir;
            if (!button->value.isEmpty())
                valueRect = ir;
            if (!button->icon.isNull()) {
                iconRect = ir;
                iconRect.setTop(ir.top() + (ir.height() - button->iconSize.height()) / 2);
                iconRect.setWidth(button->iconSize.width());
                iconRect.setHeight(button->iconSize.height());
                textRect.setLeft(iconRect.right() + spacing);
                valueRect.setLeft(textRect.left());
            }

            QFont valuefont = painter->font();
            QFontMetrics valuefm = button->fontMetrics;

            if (button->styles & QStyleOptionMaemo5ValueButton::ValueUnderText) {
                valuefont = standardFont(QLS("SmallSystemFont"));
                valuefm = QFontMetrics(valuefont);
            }

            QSize textSize = button->fontMetrics.size(Qt::TextSingleLine, button->text);
            QSize valueSize = valuefm.size(Qt::TextSingleLine, button->value);

            if (textRect.isValid())
                textRect.setWidth(textSize.width());
            if (valueRect.isValid())
                valueRect.setWidth(valueSize.width());

            if (button->styles & QStyleOptionMaemo5ValueButton::ValueUnderText) {
                if (valueRect.isValid() && textRect.isValid()) {
                    int delta = (ir.height() - (textSize.height() + valueSize.height())) / 2;
                    textRect.setTop(textRect.top() + delta);
                    textRect.setHeight(textSize.height());
                    valueRect.setTop(textRect.bottom() + 1);
                    valueRect.setHeight(valueSize.height());
                }
                if (button->styles & QStyleOptionMaemo5ValueButton::Centered) {
                    int deltaText = (textRect.width() - valueRect.width()) / 2;
                    int offsetX = ir.left() + (ir.width() - ((iconRect.isValid() ? iconRect.width() + spacing : 0) + qMax(textRect.width(), valueRect.width()))) / 2;
                    if (iconRect.isValid()) {
                        iconRect.moveLeft(offsetX);
                        offsetX += (iconRect.width() + spacing);
                    }
                    if (textRect.isValid())
                        textRect.moveLeft(offsetX + (deltaText > 0 ? 0 : -deltaText));
                    if (valueRect.isValid())
                        valueRect.moveLeft(offsetX + (deltaText < 0 ? 0 : deltaText));
                }
            } else {
                int middle = ir.center().x();
                if (textRect.isValid() && valueRect.isValid()) {
                    if ((textRect.right() + spacing) < middle)
                        valueRect.moveLeft(middle);
                    else
                        valueRect.moveLeft(textRect.right() + spacing);
                }
            }
            valueRect = visualRect(button->direction, ir, valueRect & ir);
            textRect = visualRect(button->direction, ir, textRect & ir);
            iconRect = visualRect(button->direction, ir, iconRect & ir);

            //qDebug() << "Icon, Text, Value: " << iconRect << textRect << valueRect;

            if (iconRect.isValid()) {
                QIcon::Mode mode = button->state & State_Enabled ? QIcon::Normal
                                                              : QIcon::Disabled;
                QIcon::State state = QIcon::Off;
                if (button->state & State_On)
                    state = QIcon::On;

                QPixmap pixmap = button->icon.pixmap(button->iconSize, mode, state);
                painter->drawPixmap(iconRect, pixmap);
            }
            if (textRect.isValid()) {
                proxy()->drawItemText(painter, textRect, tf, button->palette, (button->state & State_Enabled),
                                      button->text, QPalette::ButtonText);
            }
            if (valueRect.isValid()) {
                QPalette pal = button->palette;
                if (!(button->state & (State_On | State_Sunken)) && (button->state & State_Enabled)) {
                    QColor color = standardColor(button->styles & QStyleOptionMaemo5ValueButton::PickButton ? QLS("ActiveTextColor") : QLS("SecondaryTextColor"));
                    pal.setBrush(QPalette::ButtonText, color);
                }
                QFont oldfont = painter->font();
                painter->setFont(valuefont);

                proxy()->drawItemText(painter, valueRect, tf, pal, (button->state & State_Enabled),
                                      button->value, QPalette::ButtonText);
                painter->setFont(oldfont);
            }
        } else if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            QRect ir = button->rect;
            uint tf = Qt::AlignVCenter | Qt::TextHideMnemonic;

            if (!button->icon.isNull()) {
                //Center both icon and text
                QPoint point;

                QIcon::Mode mode = button->state & State_Enabled ? QIcon::Normal
                                                              : QIcon::Disabled;
                QIcon::State state = QIcon::Off;
                if (button->state & State_On)
                    state = QIcon::On;

                QPixmap pixmap = button->icon.pixmap(button->iconSize, mode, state);
                int w = pixmap.width();
                int h = pixmap.height();

                if (!button->text.isEmpty())
                    w += button->fontMetrics.boundingRect(option->rect, tf, button->text).width() + 2;

                point = QPoint(ir.x() + ir.width() / 2 - w / 2,
                               ir.y() + ir.height() / 2 - h / 2);

                if (button->direction == Qt::RightToLeft)
                    point.rx() += pixmap.width();

                painter->drawPixmap(visualPos(button->direction, button->rect, point), pixmap);

                if (button->direction == Qt::RightToLeft)
                    ir.translate(-point.x() - 2, 0);
                else
                    ir.translate(point.x() + pixmap.width(), 0);

                // left-align text if there is
                if (!button->text.isEmpty())
                    tf |= Qt::AlignLeft;

            } else {
                tf |= Qt::AlignHCenter;
            }

            if (button->features & QStyleOptionButton::HasMenu)
                ir = ir.adjusted(0, 0, -pixelMetric(PM_MenuButtonIndicator, button, widget), 0);
            proxy()->drawItemText(painter, ir, tf, button->palette, (button->state & State_Enabled),
                         button->text, QPalette::ButtonText);
        }
        break;

    case CE_ComboBoxLabel:
        if (const QStyleOptionComboBox *combo = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
            if (!combo->editable) {
                QStyleOptionMaemo5ValueButton button;
                button.QStyleOption::operator=(*option); // copy QStyleOption members
                button.features = QStyleOptionButton::None;
                button.icon = combo->currentIcon;
                button.iconSize = combo->iconSize;
                button.text = QString();
                button.styles = QStyleOptionMaemo5ValueButton::ValueBesideText | QStyleOptionMaemo5ValueButton::PickButton;
                button.value = combo->currentText;
                button.rect = proxy()->subElementRect(QStyle::SE_PushButtonContents, &button, widget);

                proxy()->drawControl(CE_PushButtonLabel, &button, painter, widget);
            } else {
                QRect editRect = proxy()->subControlRect(CC_ComboBox, combo, SC_ComboBoxEditField, widget);
                painter->save();
                painter->setClipRect(editRect);

                if (!combo->currentIcon.isNull()) {
                    QIcon::Mode mode = combo->state & State_Enabled ? QIcon::Normal
                                       : QIcon::Disabled;
                    QPixmap pixmap = combo->currentIcon.pixmap(combo->iconSize, mode);
                    QRect iconRect(editRect);
                    iconRect.setWidth(combo->iconSize.width() + 4);

                    iconRect = alignedRect(combo->direction,
                                           Qt::AlignLeft | Qt::AlignVCenter,
                                           iconRect.size(), editRect);

                    proxy()->drawItemPixmap(painter, iconRect, Qt::AlignCenter, pixmap);

                    if (combo->direction == Qt::RightToLeft)
                        editRect.translate(-4 - combo->iconSize.width(), 0);
                    else
                        editRect.translate(combo->iconSize.width() + 4, 0);
                }
                painter->restore();
            }
        }
        break;

    case CE_Splitter:
        painter->save();
        {
            QColor gripShadow(0xffffff);
            gripShadow.setAlpha(190);
            bool vertical = !(option->state & State_Horizontal);
            QRect scrollBarSlider = option->rect;
            int gripMargin = 5;
            int size = 15;
            //draw grips
            if (vertical) {
                for( int i = -size; i< size ; i += 4) {
                    painter->setPen(QPen(gripShadow, 2));
                    painter->drawLine(
                        QPoint(scrollBarSlider.center().x() + i ,
                               scrollBarSlider.top() + gripMargin),
                        QPoint(scrollBarSlider.center().x() + i,
                               scrollBarSlider.bottom() - gripMargin));
                }
            } else {
                for (int i = -size; i < size ; i += 4) {
                    painter->setPen(QPen(gripShadow, 2));
                    painter->drawLine(
                        QPoint(scrollBarSlider.left() + gripMargin ,
                               scrollBarSlider.center().y()+ i),
                        QPoint(scrollBarSlider.right() - gripMargin,
                               scrollBarSlider.center().y()+ i));
                }
            }
        }
        painter->restore();
        break;

    default:
        QGtkStyle::drawControl(element, option, painter, widget);
        break;
    }
}

/*!
    \reimp
*/
QRect QMaemo5Style::subControlRect(ComplexControl control, const QStyleOptionComplex *option,
                                   SubControl subControl, const QWidget *widget) const
{
    Q_D(const QMaemo5Style);
    QRect rect;
    switch (control) {

    case CC_SpinBox:
        if (const QStyleOptionSpinBox *spinbox = qstyleoption_cast<const QStyleOptionSpinBox *>(option)) {
            int x = spinbox->rect.x();
            int y = spinbox->rect.y();
            int h = spinbox->rect.height();
            int w = spinbox->rect.width();
            int buttonSize = 0, spacing = 0;
            if (spinbox->buttonSymbols != QAbstractSpinBox::NoButtons)
                buttonSize = 54;

            int frameOffset = buttonSize + spacing;

            GtkBorder *border = 0;
            GtkWidget *gtkEntry = d->gtkWidget("HildonNumberEditor.GtkEntry");
	    gtk_widget_style_get(gtkEntry, "inner-border", &border, NULL);
            int xframe = border->left + gtkEntry->style->xthickness + 10;
            int yframe = border->top + gtkEntry->style->ythickness + 12;
	    gtk_border_free(border);

            switch (subControl) {
            case SC_SpinBoxFrame:
                rect = spinbox->rect;
                break;

            case SC_SpinBoxEditField:
                rect = QRect(x + frameOffset + xframe, y + yframe,
                             w - 2 * (frameOffset + xframe),
                             h - 2 * yframe);
                break;

            case SC_SpinBoxUp:
                rect = QRect(x + w - buttonSize, y, buttonSize, h);
                break;

            case SC_SpinBoxDown:
                rect = QRect(x, y, buttonSize, h);
                break;

            default:
                break;

            }
            rect = visualRect(spinbox->direction, spinbox->rect, rect);
        }
        break;
    case CC_ScrollBar:
        if (const QStyleOptionSlider *scrollbar = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            const QRect scrollBarRect = scrollbar->rect;
            int maxlen = ((scrollbar->orientation == Qt::Horizontal) ?
                          scrollBarRect.width() : scrollBarRect.height());
            int sliderlen;

            // calculate slider length
            if (scrollbar->maximum != scrollbar->minimum) {
                uint range = scrollbar->maximum - scrollbar->minimum;
                sliderlen = (qint64(scrollbar->pageStep) * maxlen) / (range + scrollbar->pageStep);

                int slidermin = proxy()->pixelMetric(PM_ScrollBarSliderMin, scrollbar, widget);
                if (sliderlen < slidermin || range > INT_MAX / 2)
                    sliderlen = slidermin;
                if (sliderlen > maxlen)
                    sliderlen = maxlen;
            } else {
                sliderlen = maxlen;
            }

            int sliderstart = sliderPositionFromValue(scrollbar->minimum,
                                                      scrollbar->maximum,
                                                      scrollbar->sliderPosition,
                                                      maxlen - sliderlen,
                                                      scrollbar->upsideDown);

            switch (subControl) {
            case SC_ScrollBarSubLine:            // top/left button
            case SC_ScrollBarAddLine:            // bottom/right button
                rect = QRect();
                break;
            case SC_ScrollBarSubPage:            // between top/left button and slider
                if (scrollbar->orientation == Qt::Horizontal)
                    rect.setRect(0, 0, sliderstart, scrollBarRect.height());
                else
                    rect.setRect(0, 0, scrollBarRect.width(), sliderstart);
                break;
            case SC_ScrollBarAddPage:            // between bottom/right button and slider
                if (scrollbar->orientation == Qt::Horizontal)
                    rect.setRect(sliderstart + sliderlen, 0,
                              maxlen - sliderstart - sliderlen, scrollBarRect.height());
                else
                    rect.setRect(0, sliderstart + sliderlen, scrollBarRect.width(),
                              maxlen - sliderstart - sliderlen);
                break;
            case SC_ScrollBarGroove:
                rect = scrollBarRect;
                break;
            case SC_ScrollBarSlider:
                if (scrollbar->orientation == Qt::Horizontal)
                    rect.setRect(sliderstart, 0, sliderlen, scrollBarRect.height());
                else
                    rect.setRect(0, sliderstart, scrollBarRect.width(), sliderlen);
                break;
            default:
                break;
            }
            rect = visualRect(scrollbar->direction, scrollBarRect, rect);
        }
        break;

    case CC_ComboBox:
        if (const QStyleOptionComboBox *box = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
            if (!box->editable) {
                switch (subControl) {
                case SC_ComboBoxArrow:
                    rect = QRect(option->rect.topLeft(), QSize(0, 0));
                    break;
                default:
                    rect = option->rect;
                    break;
                }
                break;
            }

            // We employ the gtk widget to position arrows and separators for us
            GtkWidget *gtkCombo = d->gtkWidget("hildon-combobox");
	    gtk_widget_set_direction(gtkCombo, (option->direction == Qt::RightToLeft) ? GTK_TEXT_DIR_RTL : GTK_TEXT_DIR_LTR);
            GtkAllocation geometry = {0, 0, qMax(0, option->rect.width()), qMax(0, option->rect.height())};
	    gtk_widget_size_allocate(gtkCombo, &geometry);

            GtkWidget *arrowWidget = d->gtkWidget("hildon-combobox.hildon-combobox-button");
            GtkWidget *entryWidget = d->gtkWidget("hildon-combobox.HildonEntry-finger");
            if (!arrowWidget || !entryWidget)
                return QCleanlooksStyle().subControlRect(control, option, subControl, widget);

            QRect buttonRect(option->rect.left() + arrowWidget->allocation.x,
                             option->rect.top() + arrowWidget->allocation.y,
                             arrowWidget->allocation.width, arrowWidget->allocation.height);

            switch (subControl) {

            case SC_ComboBoxArrow: // Note: this indicates the arrowbutton for editable combos
                rect = buttonRect;
                break;

            case SC_ComboBoxEditField: {
                rect = visualRect(option->direction, option->rect, rect);
                int xMargin = 8, yMargin = 12;
                rect.setRect(option->rect.left() + entryWidget->style->xthickness + xMargin,
                             option->rect.top()  + entryWidget->style->ythickness + yMargin,
                             option->rect.width() - buttonRect.width() - 2*(entryWidget->style->xthickness + xMargin),
                             option->rect.height() - 2*(entryWidget->style->ythickness + yMargin));
                rect = visualRect(option->direction, option->rect, rect);
                break;
            }

            default: {
                QStyle *winstyle = QStyleFactory::create("windows");
                rect = winstyle->subControlRect(control, option, subControl, widget);
                delete winstyle;
                break;
            }
            }
        }

        break;

    default:
        rect = QGtkStyle::subControlRect(control, option, subControl, widget);
        break;
    }
    return rect;
}

/*!
    \reimp
*/
QSize QMaemo5Style::sizeFromContents(ContentsType type, const QStyleOption *option,
                                  const QSize &size, const QWidget *widget) const
{
    Q_D(const QMaemo5Style);

    QSize newSize = QGtkStyle::sizeFromContents(type, option, size, widget);
    if (!d->isThemeAvailable())
        return newSize;

    GtkWidget *gtkWidget = 0;

    switch (type) {
    case CT_ToolButton:
        if (const QStyleOptionToolButton *toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(option)) {
            // tool buttons are always at least as high as the icon (even if it only has a text)
            // the 78/70 is a hack, since these values are also hardcoded in Hildon
            newSize = newSize.expandedTo(toolbutton->iconSize + QSize(12, 12)).expandedTo(QSize(78, 70));
        }
        break;

    case CT_LineEdit: {
        if (const QStyleOptionFrameV2 *frame = qstyleoption_cast<const QStyleOptionFrameV2 *>(option)) {
            if (!frame->lineWidth)
	    {
		// check me !!!
		return newSize;
	    }

            gtkWidget = d->gtkWidget("HildonEntry-finger");
            if (gtkWidget) {
                newSize = size + 2 * QSize(gtkWidget->style->xthickness, 1 + gtkWidget->style->ythickness);
                GtkBorder *border = 0;
		gtk_widget_style_get(gtkWidget, "inner-border", &border, NULL);
                if (border) {
                    newSize += QSize(border->left + border->right, border->top + border->bottom + 3);
		    gtk_border_free(border);
                } else {
                    newSize += QSize(4, 7); // default inner-border
                }
                //The widget size should not be adjusted automaticly if it is too small
                //otherwise the tst_QLineEdit::textMargin() fails
                gtkWidget = 0;
            }
        }
        break;
    }

    case CT_ComboBox: {
        if (const QStyleOptionComboBox *combo = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
            if (!combo->editable) {
                QStyleOptionButton button;
                button.QStyleOption::operator=(*option); // copy QStyleOption members
                button.features = QStyleOptionButton::None;
                button.icon = combo->currentIcon;
                button.iconSize = combo->iconSize;
                button.text = combo->currentText;

                newSize = proxy()->sizeFromContents(CT_PushButton, &button, size, widget);
            } else {
                GtkWidget *gtkCombo = d->gtkWidget("hildon-combobox.HildonEntry-finger");
                QRect arrowButtonRect = proxy()->subControlRect(CC_ComboBox, combo, SC_ComboBoxArrow, widget);
                newSize = size + QSize(12 + arrowButtonRect.width() + 2*gtkCombo->style->xthickness, 4 + 2*gtkCombo->style->ythickness);
                newSize = newSize.expandedTo(QSize(0, 70));
            }
        }
        break;
    }

    case CT_PushButton: {
        if (widget && qobject_cast<QDialogButtonBox *>(widget->parentWidget()))
            newSize.setWidth(qMax(newSize.width(), 174)); // hardcoded value in gtkdialog.c
        gtkWidget = d->getGtkButtonForWidget(widget);
        break;
    }

    case CT_RadioButton:
    case CT_CheckBox: {
        gtkWidget = d->getGtkButtonForWidget(widget);
        if (gtkWidget)
        {
            gint w = -1, h = -1;
            gtk_widget_get_size_request(gtkWidget, &w, &h);
            newSize = newSize.expandedTo(QSize(w, h));

            GtkBorder *border = 0;
	    gtk_widget_style_get(gtkWidget, "inner-border", &border, NULL);
            if (border)
            {
                newSize += QSize(border->left + border->right,0);
		gtk_border_free(border);
            }
            gtkWidget = 0;
        }
        break;
    }

    case CT_MenuItem: {
        if (const QStyleOptionMenuItem *menuItem = qstyleoption_cast<const QStyleOptionMenuItem *>(option)) {
            int textMargin = 8;

            if (menuItem->menuItemType == QStyleOptionMenuItem::Separator) {
                GtkWidget *gtkMenuSeparator = d->gtkWidget("hildon-context-sensitive-menu.GtkSeparatorMenuItem");
                GtkRequisition sizeReq = {0, 0};
		gtk_widget_size_request(gtkMenuSeparator, &sizeReq);
                newSize = QSize(size.width(), sizeReq.height);
                break;
            }

            GtkWidget *gtkMenuItem = d->gtkWidget("hildon-context-sensitive-menu.GtkCheckMenuItem");
            GtkStyle* style = gtkMenuItem->style;

            // Note we get the perfect height for the default font since we
            // set a fake text label on the gtkMenuItem
            // But if custom fonts are used on the widget we need a minimum size
            GtkRequisition sizeReq = {0, 0};
	    gtk_widget_size_request(gtkMenuItem, &sizeReq);
            newSize.setHeight(qMax(newSize.height() - 4, sizeReq.height));
            newSize += QSize(textMargin + style->xthickness - 1, 0);

            // Cleanlooks assumes a check column of 20 pixels so we need to
            // expand it a bit
            gint checkSize;
	    gtk_widget_style_get(gtkMenuItem, "indicator-size", &checkSize, NULL);
            newSize.setWidth(newSize.width() + qMax(0, checkSize - 20));
        }
        break;
    }
#ifndef QT_NO_ITEMVIEWS
    case CT_ItemViewItem: {
         uint rowHeight = 70;
         if (GtkWidget *gtkTreeView = d->gtkWidget("HildonPannableArea.GtkTreeView"))
	     gtk_widget_style_get(gtkTreeView, "row-height", &rowHeight, NULL);
         newSize = newSize.expandedTo(QSize(0, rowHeight));
         break;
    }
#endif // QT_NO_ITEMVIEWS
    case CT_ProgressBar: {
        gint minw = 70, minh = 70;
        if (GtkWidget *gtkProgressBar = d->gtkWidget("GtkProgressBar")) {
	    gtk_widget_style_get(gtkProgressBar,
                                   "min-vertical-bar-width",    &minw,
                                   "min-horizontal-bar-height", &minh,
                                   NULL);
        }
        newSize = newSize.expandedTo(QSize(minw, minh));
        break;
    }

    case CT_Slider: {
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            // I have no idea why the Gtk widget ends up being 70px high given the Hildon gtkrc and the gtkscale source code...
            QSize expandFix(0, 70);
            if (slider->orientation == Qt::Horizontal) {
                gtkWidget = d->gtkWidget("GtkHScale");
            } else {
                gtkWidget = d->gtkWidget("GtkVScale");
                expandFix.transpose();
            }
            newSize = newSize.expandedTo(expandFix);
        }
        break;
    }

    default:
        break;
    }

    if (gtkWidget) {
        gint w = -1, h = -1;
	gtk_widget_get_size_request(gtkWidget, &w, &h);
        newSize = newSize.expandedTo(QSize(w, h));
    }
    return newSize;
}

/*!
    \reimp
*/
QRect QMaemo5Style::subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const
{
    Q_D(const QMaemo5Style);

    QRect r;
    switch (element) {
    case SE_TreeViewDisclosureItem:
        // Increases the actual expander hitrect for more finger friendliness
        r = option->rect.adjusted(0, 0, 20, 0);
        break;

    case SE_LineEditContents:
        if (const QStyleOptionFrame *frame = qstyleoption_cast<const QStyleOptionFrame *>(option)) {
            if (!frame->lineWidth)
        return QCleanlooksStyle().subElementRect(element, option, widget);
        }
        r = option->rect;
        if (GtkWidget *gtkEntry = d->gtkWidget("HildonEntry-finger")) {
            int xt = gtkEntry->style->xthickness;
            int yt = gtkEntry->style->ythickness;

            r.adjust(xt, yt, -xt, -yt);

            GtkBorder *border = 0;
	    gtk_widget_style_get(gtkEntry, "inner-border", &border, NULL);
            if (border) {
                r.adjust(border->left, border->top, -border->right, -border->bottom);
		gtk_border_free(border);
            } else {
                r.adjust(2, 2, -2, -2); // default inner-border
            }
        }
        r = visualRect(option->direction, option->rect, r);
        break;

    case SE_CheckBoxIndicator: {
        int h = proxy()->pixelMetric(PM_IndicatorHeight, option, widget);
        r.setRect(option->rect.x(), option->rect.y() + ((option->rect.height() - h) / 2),
                  proxy()->pixelMetric(PM_IndicatorWidth, option, widget), h);
        r = visualRect(option->direction, option->rect, r);
        break;
    }
    case SE_RadioButtonContents:
        r = proxy()->subElementRect(SE_PushButtonContents, option, widget);
        break;

    case SE_RadioButtonClickRect: // fall through
    case SE_CheckBoxClickRect:
        r = visualRect(option->direction, option->rect, option->rect);
        break;

    case SE_FrameContents:
        if ((qobject_cast<const QTextEdit *>(widget) && !qobject_cast<const QTextBrowser *>(widget)) || qobject_cast<const QPlainTextEdit *>(widget))
            r = proxy()->subElementRect(SE_LineEditContents, option, widget);
        else
            r = QGtkStyle::subElementRect(element, option, widget);
        break;

    default:
        r = QGtkStyle::subElementRect(element, option, widget);
    }
    return r;
}

/*!
    \reimp
*/
QStyle::SubControl QMaemo5Style::hitTestComplexControl(ComplexControl cc, const QStyleOptionComplex *opt,
                              const QPoint &pt, const QWidget *w) const
{
    switch (cc) {
    case CC_ScrollBar:
        return SC_None;
    default:
        return QGtkStyle::hitTestComplexControl(cc, opt, pt, w);
    }
}

/*!
    Returns a font for the given \a logicalFontName.

    Please see the Maemo 5 style guides for a list of valid logical font
    names.  If the named font cannot be found, the function returns the
    application's default font.

    \sa standardColor()
*/
QFont QMaemo5Style::standardFont(const QString &logicalFontName)
{
    return QMaemo5StylePrivate::getThemeFont(logicalFontName);
}

/*!
    Returns a color for the given \a logicalColorName.

    Please see the Maemo 5 style guides for a list of valid logical color
    names.  If the named color cannot be found, the function returns an
    invalid color.

    \sa standardFont()
*/
QColor QMaemo5Style::standardColor(const QString &logicalColorName)
{
    if (GtkStyle *style = QGtkStylePrivate::gtkStyle()) {
        GdkColor color;
        if (gtk_style_lookup_color(style, logicalColorName.toUtf8().constData(), &color))
            return QColor(color.red>>8, color.green>>8, color.blue>>8);
    }
    return QColor();
}


/*!
    \enum QMaemo5Style::PrimitiveElement
    \since 4.6

    This enum contains additional Maemo 5 specific PrimitiveElement entries.

    \value PE_Maemo5InformationBox Represents a Maemo 5 information box overlay.

    \value PE_Maemo5AppMenu        Represents a Maemo 5 application menu frame.

    \value PE_Maemo5EditBar        Represents the special Maemo 5 edit bar pop-up background.

    \value PE_Maemo5EditBarBackButton  Represents the back button for the edit bar.

    \sa QStyle::PrimitiveElement
*/

/*!
    \enum QMaemo5Style::PixelMetric
    \since 4.6

    This enum contains additional Maemo 5 specific PixelMetric entries.

    \value PM_Maemo5AppMenuHorizontalSpacing The horizontal space between entries inside the Maemo 5 application menu.

    \value PM_Maemo5AppMenuVerticalSpacing   The vertical space between entries inside the Maemo 5 application menu.

    \value PM_Maemo5AppMenuContentMargin     The space around the Maemo 5 app menu content.

    \value PM_Maemo5AppMenuLandscapeXOffset  The space from the edge of the screen to the menu border.

    \value PM_Maemo5AppMenuFilterGroupWidth   The width of the Maemo5 menu filter group.

    \value PM_Maemo5AppMenuFilterGroupVerticalSpacing The space between the filter group and the rest of the Maemo5 menu.

    \value PM_Maemo5EditBarBackButtonWidth   The width of the Maemo5 edit bar back button.

    \value PM_Maemo5EditBarBackButtonHeight  The height of the Maemo5 edit bar back button.

    \sa QStyle::PixelMetric
*/

/*! \internal */
QIcon QMaemo5Style::standardIconImplementation(StandardPixmap standardIcon,
        const QStyleOption *option, const QWidget *widget) const
{
    switch (standardIcon) {
    case SP_ArrowLeft:
        return QIcon::fromTheme(QLatin1String("general_back"));
    case SP_ArrowRight:
        return QIcon::fromTheme(QLatin1String("general_forward"));
    default:
        return standardPixmap(standardIcon, option, widget);
    }
}

/*! \reimp */
QPixmap QMaemo5Style::standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt,
                       const QWidget *widget) const
{
    switch (standardPixmap) {
    case SP_MediaPlay:
        return QPixmap(QLatin1String("/etc/hildon/theme/mediaplayer/Play.png"));
    case SP_MediaStop:
        return QPixmap(QLatin1String("/etc/hildon/theme/mediaplayer/Stop.png"));
    case SP_MediaPause:
        return QPixmap(QLatin1String("/etc/hildon/theme/mediaplayer/Pause.png"));
    case SP_MediaSkipForward:
        return QPixmap(QLatin1String("/etc/hildon/theme/mediaplayer/Forward.png"));
    case SP_MediaSkipBackward:
        return QPixmap(QLatin1String("/etc/hildon/theme/mediaplayer/Back.png"));
    default:
        return QGtkStyle::standardPixmap(standardPixmap, opt, widget);
    }
}

QT_END_NAMESPACE
