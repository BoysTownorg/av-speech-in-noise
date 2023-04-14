// https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/drawingwithquartz2d/dq_context/dq_context.html#//apple_ref/doc/uid/TP30001066-CH203-CJBHBFFE
// https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/drawingwithquartz2d/dq_images/dq_images.html#//apple_ref/doc/uid/TP30001066-CH212-TPXREF101

#include <AppKit/AppKit.h>
#include <CoreFoundation/CoreFoundation.h>
#include <Foundation/Foundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <ImageIO/ImageIO.h>

#include <stdexcept>
#include <memory>

namespace av_speech_in_noise {
class ScopedMaskedImage {
  public:
    ScopedMaskedImage(CGImageRef image, CGImageRef mask)
        : masked{CGImageCreateWithMask(image, mask)} {
        if (masked == nullptr)
            throw std::runtime_error{"Unable to create masked image."};
    }

    ~ScopedMaskedImage() { CGImageRelease(masked); }

    CGImageRef masked;
};

class ScopedBitmapImage {
  public:
    ScopedBitmapImage(CGContextRef context)
        : image{CGBitmapContextCreateImage(context)} {
        if (image == nullptr)
            throw std::runtime_error{"Unable to create bitmap image."};
    }

    ~ScopedBitmapImage() { CGImageRelease(image); }
    CGImageRef image;
};

class ScopedImageSource {
  public:
    ScopedImageSource(CFURLRef url)
        : imageSource{CGImageSourceCreateWithURL(url, nullptr)} {
        if (imageSource == nullptr)
            throw std::runtime_error{"Unable to create image source."};
    }

    ~ScopedImageSource() { CFRelease(imageSource); }
    CGImageSourceRef imageSource;
};

class ScopedImage {
  public:
    ScopedImage(CGImageSourceRef imageSource)
        : image{CGImageSourceCreateImageAtIndex(imageSource, 0, nullptr)} {
        if (imageSource == nullptr)
            throw std::runtime_error{"Unable to create image."};
    }

    ~ScopedImage() { CGImageRelease(image); }
    CGImageRef image;
};

static CGContextRef createBitmapContext(int pixelsWide, int pixelsHigh) {
    const auto bitmapBytesPerRow = (pixelsWide * 3); // 1
    const auto colorSpace = CGColorSpaceCreateDeviceGray(); // 2
    const auto context = CGBitmapContextCreate(nullptr, // 4
        pixelsWide, pixelsHigh,
        8, // bits per component
        bitmapBytesPerRow, colorSpace, kCGImageAlphaNone);
    CGColorSpaceRelease(colorSpace);
    return context;
};

class ScopedBitmapContext {
  public:
    ScopedBitmapContext(CGImageRef image)
        : context{createBitmapContext(
              CGImageGetWidth(image), CGImageGetHeight(image))} {
        if (context == nullptr)
            throw std::runtime_error{"Unable to create bitmap context"};
    }

    ~ScopedBitmapContext() { CGContextRelease(context); }
    CGContextRef context;
};
}

@interface ImageView : NSView
- (instancetype)initWithImage:
    (std::unique_ptr<av_speech_in_noise::ScopedMaskedImage>)image;
@end

@implementation ImageView {
    std::unique_ptr<av_speech_in_noise::ScopedMaskedImage> _image;
}
- (id)initWithImage:
    (std::unique_ptr<av_speech_in_noise::ScopedMaskedImage>)image {
    self = [super initWithFrame:NSMakeRect(0, 0, CGImageGetWidth(image->masked),
                                    CGImageGetHeight(image->masked))];
    if (self) {
        _image = std::move(image);
    }
    return self;
}

- (void)drawRect:(NSRect)rect {
    CGContextRef myContext = [[NSGraphicsContext currentContext] CGContext];
    CGContextDrawImage(myContext,
        CGRectMake(0, 0, CGImageGetWidth(_image->masked),
            CGImageGetHeight(_image->masked)),
        _image->masked);
}
@end

@interface AppDelegate : NSObject <NSApplicationDelegate>

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    NSURL *url = [[NSBundle mainBundle] URLForImageResource:@"wally.jpg"];
    if (url == nil) {
        NSLog(@"URL is nil. exiting.");
        return;
    }
    const auto imageSource{
        av_speech_in_noise::ScopedImageSource{(__bridge CFURLRef)(url)}};
    NSLog(@"image count: %zu", CGImageSourceGetCount(imageSource.imageSource));
    const auto image{av_speech_in_noise::ScopedImage{imageSource.imageSource}};
    NSLog(@"image width: %zu", CGImageGetWidth(image.image));
    const auto context{av_speech_in_noise::ScopedBitmapContext{image.image}};
    CGContextSetRGBFillColor(context.context, 1, 1, 1, 1);
    CGContextFillRect(context.context, CGRectMake(0, 0, 100, 100));
    CGContextSetRGBFillColor(context.context, 1, 1, 1, 1);
    CGContextFillRect(context.context, CGRectMake(100, 100, 100, 100));
    CGContextSetRGBFillColor(context.context, 1, 1, 1, 0);
    CGContextFillRect(context.context, CGRectMake(0, 100, 100, 100));
    CGContextSetRGBFillColor(context.context, 0, 0, 0, 0);
    CGContextFillRect(context.context, CGRectMake(100, 0, 100, 100));
    const auto mask{av_speech_in_noise::ScopedBitmapImage{context.context}};

    NSTabViewController *controller = [[NSTabViewController alloc] init];
    [controller setTabStyle:NSTabViewControllerTabStyleUnspecified];
    NSWindow *window = [NSWindow windowWithContentViewController:controller];
    [window makeKeyAndOrderFront:nil];
    ImageView *view = [[ImageView alloc]
        initWithImage:std::make_unique<av_speech_in_noise::ScopedMaskedImage>(
                          image.image, mask.image)];
    [window.contentView addSubview:view];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

- (BOOL)applicationSupportsSecureRestorableState:(NSApplication *)app {
    return YES;
}

@end
