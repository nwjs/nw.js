// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "chrome/browser/ui/cocoa/autofill/autofill_popup_view_cocoa.h"

#include "base/logging.h"
#include "base/strings/sys_string_conversions.h"
#include "chrome/browser/ui/autofill/autofill_popup_controller.h"
#include "chrome/browser/ui/autofill/popup_constants.h"
#include "chrome/browser/ui/cocoa/autofill/autofill_popup_view_bridge.h"
#include "components/autofill/core/browser/popup_item_ids.h"
#include "ui/base/cocoa/window_size_constants.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/gfx/font_list.h"
#include "ui/gfx/image/image.h"
#include "ui/gfx/point.h"
#include "ui/gfx/rect.h"

using autofill::AutofillPopupView;

@interface AutofillPopupViewCocoa ()

#pragma mark -
#pragma mark Private methods

// Draws an Autofill suggestion in the given |bounds|, labeled with the given
// |name| and |subtext| hint.  If the suggestion |isSelected|, then it is drawn
// with a highlight.  |index| determines the font to use, as well as the icon,
// if the row requires it -- such as for credit cards. |imageFirst| indicates
// whether the image should be drawn before the name, and with the same
// alignment, or whether it should be drawn afterwards, with the opposite
// alignment.
- (void)drawSuggestionWithName:(NSString*)name
                       subtext:(NSString*)subtext
                         index:(size_t)index
                        bounds:(NSRect)bounds
                      selected:(BOOL)isSelected
                    imageFirst:(BOOL)imageFirst
                   textYOffset:(CGFloat)textYOffset;

// This comment block applies to all three draw* methods that follow.
// If |rightAlign| == YES.
//   Draws the widget with right border aligned to |x|.
//   Returns the x value of left border of the widget.
// If |rightAlign| == NO.
//   Draws the widget with left border aligned to |x|.
//   Returns the x value of right border of the widget.
- (CGFloat)drawName:(NSString*)name
                atX:(CGFloat)x
              index:(size_t)index
         rightAlign:(BOOL)rightAlign
             bounds:(NSRect)bounds
        textYOffset:(CGFloat)textYOffset;
- (CGFloat)drawIconAtIndex:(size_t)index
                       atX:(CGFloat)x
                rightAlign:(BOOL)rightAlign
                    bounds:(NSRect)bounds;
- (CGFloat)drawSubtext:(NSString*)subtext
                   atX:(CGFloat)x
            rightAlign:(BOOL)rightAlign
                bounds:(NSRect)bounds
           textYOffset:(CGFloat)textYOffset;

// Returns the icon for the row with the given |index|, or |nil| if there is
// none.
- (NSImage*)iconAtIndex:(size_t)index;

@end

@implementation AutofillPopupViewCocoa

#pragma mark -
#pragma mark Initialisers

- (id)initWithFrame:(NSRect)frame {
  NOTREACHED();
  return [self initWithController:NULL frame:frame];
}

- (id)initWithController:(autofill::AutofillPopupController*)controller
                   frame:(NSRect)frame {
  self = [super initWithDelegate:controller frame:frame];
  if (self)
    controller_ = controller;

  return self;
}

#pragma mark -
#pragma mark NSView implementation:

- (void)drawRect:(NSRect)dirtyRect {
  // If the view is in the process of being destroyed, don't bother drawing.
  if (!controller_)
    return;

  [self drawBackgroundAndBorder];

  for (size_t i = 0; i < controller_->names().size(); ++i) {
    // Skip rows outside of the dirty rect.
    NSRect rowBounds =
        NSRectFromCGRect(controller_->GetRowBounds(i).ToCGRect());
    if (!NSIntersectsRect(rowBounds, dirtyRect))
      continue;

    if (controller_->identifiers()[i] == autofill::POPUP_ITEM_ID_SEPARATOR) {
      [self drawSeparatorWithBounds:rowBounds];
      continue;
    }

    // Additional offset applied to the text in the vertical direction.
    CGFloat textYOffset = 0;
    BOOL imageFirst = NO;
    if (controller_->identifiers()[i] ==
        autofill::POPUP_ITEM_ID_MAC_ACCESS_CONTACTS) {
      // Due to the weighting of the asset used for this autofill entry, the
      // text needs to be bumped up by 1 pt to make it look vertically aligned.
      textYOffset = -1;
      imageFirst = YES;
    }

    NSString* name = SysUTF16ToNSString(controller_->names()[i]);
    NSString* subtext = SysUTF16ToNSString(controller_->subtexts()[i]);
    BOOL isSelected = static_cast<int>(i) == controller_->selected_line();
    [self drawSuggestionWithName:name
                         subtext:subtext
                           index:i
                          bounds:rowBounds
                        selected:isSelected
                      imageFirst:imageFirst
                     textYOffset:textYOffset];
  }
}

#pragma mark -
#pragma mark Public API:

- (void)controllerDestroyed {
  // Since the |controller_| either already has been destroyed or is about to
  // be, about the only thing we can safely do with it is to null it out.
  controller_ = NULL;
  [super delegateDestroyed];
}

- (void)invalidateRow:(size_t)row {
  NSRect dirty_rect =
      NSRectFromCGRect(controller_->GetRowBounds(row).ToCGRect());
  [self setNeedsDisplayInRect:dirty_rect];
}

#pragma mark -
#pragma mark Private API:

- (void)drawSuggestionWithName:(NSString*)name
                       subtext:(NSString*)subtext
                         index:(size_t)index
                        bounds:(NSRect)bounds
                      selected:(BOOL)isSelected
                    imageFirst:(BOOL)imageFirst
                   textYOffset:(CGFloat)textYOffset {
  // If this row is selected, highlight it.
  if (isSelected) {
    [[self highlightColor] set];
    [NSBezierPath fillRect:bounds];
  }

  BOOL isRTL = controller_->IsRTL();

  // The X values of the left and right borders of the autofill widget.
  CGFloat leftX = NSMinX(bounds) + AutofillPopupView::kEndPadding;
  CGFloat rightX = NSMaxX(bounds) - AutofillPopupView::kEndPadding;

  // Draw left side if isRTL == NO, right side if isRTL == YES.
  CGFloat x = isRTL ? rightX : leftX;
  if (imageFirst)
    x = [self drawIconAtIndex:index atX:x rightAlign:isRTL bounds:bounds];
  [self drawName:name
              atX:x
            index:index
       rightAlign:isRTL
           bounds:bounds
      textYOffset:textYOffset];

  // Draw right side if isRTL == NO, left side if isRTL == YES.
  x = isRTL ? leftX : rightX;
  if (!imageFirst)
    x = [self drawIconAtIndex:index atX:x rightAlign:!isRTL bounds:bounds];
  [self drawSubtext:subtext
                atX:x
         rightAlign:!isRTL
             bounds:bounds
        textYOffset:textYOffset];
}

- (CGFloat)drawName:(NSString*)name
                atX:(CGFloat)x
              index:(size_t)index
         rightAlign:(BOOL)rightAlign
             bounds:(NSRect)bounds
        textYOffset:(CGFloat)textYOffset {
  NSColor* nameColor =
      controller_->IsWarning(index) ? [self warningColor] : [self nameColor];
  NSDictionary* nameAttributes =
      [NSDictionary dictionaryWithObjectsAndKeys:
           controller_->GetNameFontListForRow(index).GetPrimaryFont().
               GetNativeFont(),
           NSFontAttributeName, nameColor, NSForegroundColorAttributeName,
           nil];
  NSSize nameSize = [name sizeWithAttributes:nameAttributes];
  x -= rightAlign ? nameSize.width : 0;
  CGFloat y = bounds.origin.y + (bounds.size.height - nameSize.height) / 2;
  y += textYOffset;

  [name drawAtPoint:NSMakePoint(x, y) withAttributes:nameAttributes];

  x += rightAlign ? 0 : nameSize.width;
  return x;
}

- (CGFloat)drawIconAtIndex:(size_t)index
                       atX:(CGFloat)x
                rightAlign:(BOOL)rightAlign
                    bounds:(NSRect)bounds {
  NSImage* icon = [self iconAtIndex:index];
  if (!icon)
    return x;
  NSSize iconSize = [icon size];
  x -= rightAlign ? iconSize.width : 0;
  CGFloat y = bounds.origin.y + (bounds.size.height - iconSize.height) / 2;
    [icon drawInRect:NSMakeRect(x, y, iconSize.width, iconSize.height)
            fromRect:NSZeroRect
           operation:NSCompositeSourceOver
            fraction:1.0
      respectFlipped:YES
               hints:nil];

    x += rightAlign ? -AutofillPopupView::kIconPadding
                    : iconSize.width + AutofillPopupView::kIconPadding;
    return x;
}

- (CGFloat)drawSubtext:(NSString*)subtext
                   atX:(CGFloat)x
            rightAlign:(BOOL)rightAlign
                bounds:(NSRect)bounds
           textYOffset:(CGFloat)textYOffset {
  NSDictionary* subtextAttributes =
      [NSDictionary dictionaryWithObjectsAndKeys:
           controller_->subtext_font_list().GetPrimaryFont().GetNativeFont(),
           NSFontAttributeName,
           [self subtextColor],
           NSForegroundColorAttributeName,
           nil];
  NSSize subtextSize = [subtext sizeWithAttributes:subtextAttributes];
  x -= rightAlign ? subtextSize.width : 0;
  CGFloat y = bounds.origin.y + (bounds.size.height - subtextSize.height) / 2;
  y += textYOffset;

  [subtext drawAtPoint:NSMakePoint(x, y) withAttributes:subtextAttributes];
  x += rightAlign ? 0 : subtextSize.width;
  return x;
}

- (NSImage*)iconAtIndex:(size_t)index {
  if (controller_->icons()[index].empty())
    return nil;

  int iconId = controller_->GetIconResourceID(controller_->icons()[index]);
  DCHECK_NE(-1, iconId);

  ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();
  return rb.GetNativeImageNamed(iconId).ToNSImage();
}

@end
