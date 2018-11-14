// Fill out your copyright notice in the Description page of Project Settings.

#include "TryThingsOut.h"
#include "SceneView.h"
#include "Box2D.h"

FBox2D UTryThingsOut::ConvertBoundBox2D(FMinimalViewInfo ViewInfo, FBox2D Resolution, FVector Origin, FVector Extend, TArray<FVector2D>& Pixels, TArray<FVector>& Points)
{
	Points.Add(Origin + FVector(Extend.X, Extend.Y, Extend.Z));
	Points.Add(Origin + FVector(-Extend.X, Extend.Y, Extend.Z));
	Points.Add(Origin + FVector(Extend.X, -Extend.Y, Extend.Z));
	Points.Add(Origin + FVector(-Extend.X, -Extend.Y, Extend.Z));
	Points.Add(Origin + FVector(Extend.X, Extend.Y, -Extend.Z));
	Points.Add(Origin + FVector(-Extend.X, Extend.Y, -Extend.Z));
	Points.Add(Origin + FVector(Extend.X, -Extend.Y, -Extend.Z));
	Points.Add(Origin + FVector(-Extend.X, -Extend.Y, -Extend.Z));

	FVector2D MinPixel(Resolution.Max);
	FVector2D MaxPixel(Resolution.Min);
	FIntRect ScreenRect(MaxPixel.X, MaxPixel.Y, MinPixel.X, MinPixel.Y);
	FMatrix ProjectionMatrix = ViewInfo.CalculateProjectionMatrix();
	//FTransform ViewTransform(Info.Rotation, Info.Location, FVector(1, 1, 1));
	//FMatrix ViewMatrix(ViewTransform.ToMatrixNoScale());
	for (FVector& Point : Points) {
		FVector2D Pixel;
		FSceneView::ProjectWorldToScreen(Point, ScreenRect, ProjectionMatrix, Pixel);
		Pixels.Add(Pixel);
		MaxPixel.X = FMath::Max(Pixel.X, MaxPixel.X);
		MaxPixel.Y = FMath::Max(Pixel.Y, MaxPixel.Y);
		MinPixel.X = FMath::Min(Pixel.X, MinPixel.X);
		MinPixel.Y = FMath::Min(Pixel.Y, MinPixel.Y);
	}

	return FBox2D(MinPixel, MaxPixel);
}

FBox2D UTryThingsOut::calcBoundingFromBinary(UTextureRenderTarget2D * RenderTexture)
{

	TArray<FLinearColor> ImageData;
	FRenderTarget *RenderTarget = RenderTexture->GameThread_GetRenderTargetResource();
	RenderTarget->ReadLinearColorPixels(ImageData);

	FVector2D maxPoint(0, 0);
	FVector2D minPoint(RenderTexture->SizeX, RenderTexture->SizeY);

	for (int x = 0; x < RenderTexture->SizeX; x++) {
		for (int y = 0; y < RenderTexture->SizeY; y++) {
			int i = x + y * RenderTexture->SizeX;
			if (ImageData[i].R > 0.1) {
				// we a have white pixel
				if (x <= minPoint.X) {
					minPoint.X = x;
				}
				if (y <= minPoint.Y) {
					minPoint.Y = y;
				}
				if (x >= maxPoint.X) {
					maxPoint.X = x;
				}
				if (y >= maxPoint.Y) {
					maxPoint.Y = y;
				}
			}
		}
	}
	FBox2D box(minPoint, maxPoint);

	return box;
}