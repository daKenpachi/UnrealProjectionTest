// Fill out your copyright notice in the Description page of Project Settings.

#include "TryThingsOut.h"
#include "SceneView.h"
#include "Runtime/Engine/Public/TextureResource.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2D.h"
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

bool UTryThingsOut::calcBoundingFromBox(USceneCaptureComponent2D * RenderComponent, FBox BoundingBox3D, FBox2D & BoxOut, TArray<FVector>& Points, TArray<FVector2D>& Points2D)
{
	bool isCompletelyInView = true;
	UTextureRenderTarget2D* RenderTexture = RenderComponent->TextureTarget;
	FRenderTarget *RenderTarget = RenderTexture->GameThread_GetRenderTargetResource();

	FMinimalViewInfo Info;
	Info.Location = RenderComponent->GetComponentTransform().GetLocation();
	Info.Rotation = FRotator(RenderComponent->GetComponentTransform().GetRotation());
	Info.FOV = RenderComponent->FOVAngle;
	Info.ProjectionMode = RenderComponent->ProjectionType;
	Info.AspectRatio = float(RenderTexture->SizeX) / float(RenderTexture->SizeY);

	FVector Extend = (BoundingBox3D.Max - BoundingBox3D.Min) / 2;
	FVector Origin = BoundingBox3D.Min + Extend;
	//FBox Box3D(Origin - Extend, Origin + Extend);
	//FVector cornerPoint = Box3D.GetExtrema(i);

	Points.Add(Origin + FVector(Extend.X, Extend.Y, Extend.Z));
	Points.Add(Origin + FVector(-Extend.X, Extend.Y, Extend.Z));
	Points.Add(Origin + FVector(Extend.X, -Extend.Y, Extend.Z));
	Points.Add(Origin + FVector(-Extend.X, -Extend.Y, Extend.Z));
	Points.Add(Origin + FVector(Extend.X, Extend.Y, -Extend.Z));
	Points.Add(Origin + FVector(-Extend.X, Extend.Y, -Extend.Z));
	Points.Add(Origin + FVector(Extend.X, -Extend.Y, -Extend.Z));
	Points.Add(Origin + FVector(-Extend.X, -Extend.Y, -Extend.Z));

	FVector2D MinPixel(RenderTexture->SizeX, RenderTexture->SizeY);
	FVector2D MaxPixel(0, 0);

	FIntRect ScreenRect(0, 0, RenderTexture->SizeX / 4, RenderTexture->SizeY / 4);
	FTransform ViewTransform = RenderComponent->GetComponentTransform();
	FMatrix ViewMatrix(ViewTransform.ToMatrixWithScale());
	FMatrix ProjectionMatrix = Info.CalculateProjectionMatrix();
	for (FVector& Point : Points) {
		FVector2D Pixel;
		FSceneView::ProjectWorldToScreen((Point), ScreenRect, ProjectionMatrix, Pixel);
		Points2D.Add(Pixel);
		MaxPixel.X = FMath::Max(Pixel.X, MaxPixel.X);
		MaxPixel.Y = FMath::Max(Pixel.Y, MaxPixel.Y);
		MinPixel.X = FMath::Min(Pixel.X, MinPixel.X);
		MinPixel.Y = FMath::Min(Pixel.Y, MinPixel.Y);
	}

	BoxOut = FBox2D(MinPixel, MaxPixel);
	// clamp min point
	if (BoxOut.Min.X < 0) {
		BoxOut.Min.X = 0;
		isCompletelyInView = false;
	}
	else if (BoxOut.Min.X > RenderTexture->SizeX) {
		BoxOut.Min.X = RenderTexture->SizeX;
		isCompletelyInView = false;
	}
	if (BoxOut.Min.Y < 0) {
		BoxOut.Min.Y = 0;
		isCompletelyInView = false;
	}
	else if (BoxOut.Min.Y > RenderTexture->SizeY) {
		BoxOut.Min.Y = RenderTexture->SizeY;
		isCompletelyInView = false;
	}
	// clamp max point
	if (BoxOut.Max.X > RenderTexture->SizeX) {
		BoxOut.Max.X = RenderTexture->SizeX;
		isCompletelyInView = false;
	}
	else if (BoxOut.Max.X < 0) {
		BoxOut.Max.X = 0;
		isCompletelyInView = false;
	}
	if (BoxOut.Max.Y > RenderTexture->SizeY) {
		BoxOut.Max.Y = RenderTexture->SizeY;
		isCompletelyInView = false;
	}
	else if (BoxOut.Max.Y < 0) {
		BoxOut.Max.Y = 0;
		isCompletelyInView = false;
	}
	return isCompletelyInView;
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

FBox2D UTryThingsOut::calcBoundingFromColor(UTextureRenderTarget2D * RenderTexture, FLinearColor Color)
{
	TArray<FLinearColor> ImageData;
	FRenderTarget *RenderTarget = RenderTexture->GameThread_GetRenderTargetResource();
	RenderTarget->ReadLinearColorPixels(ImageData);

	FVector2D maxPoint(0, 0);
	FVector2D minPoint(RenderTexture->SizeX, RenderTexture->SizeY);

	for (int x = 0; x < RenderTexture->SizeX; x++) {
		for (int y = 0; y < RenderTexture->SizeY; y++) {
			int i = x + y * RenderTexture->SizeX;
			if (ImageData[i].G == (Color.G)) {
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


void UTryThingsOut::RenderTarget2PointCloudWithView(USceneCaptureComponent2D* CaptureComponent, FBox2D BoundingBox, TArray<FVector>& PointCloud)
{
	double start = FPlatformTime::Seconds();
	PointCloud.Reset();



	UTextureRenderTarget2D* RenderTexture = CaptureComponent->TextureTarget;
	TArray<FLinearColor> FormatedImageData;
	FRenderTarget *RenderTarget = RenderTexture->GameThread_GetRenderTargetResource();
	RenderTarget->ReadLinearColorPixels(FormatedImageData);

	FMinimalViewInfo ViewInfo;
	ViewInfo.Location = CaptureComponent->GetComponentTransform().GetLocation();
	ViewInfo.Rotation = FRotator(CaptureComponent->GetComponentTransform().GetRotation());
	ViewInfo.FOV = CaptureComponent->FOVAngle;
	ViewInfo.ProjectionMode = CaptureComponent->ProjectionType;
	ViewInfo.AspectRatio = float(RenderTexture->SizeX) / float(RenderTexture->SizeY);

	// lambda for array access
	auto toIndex = [RenderTexture](int x, int y) {
		return x + y * RenderTexture->SizeX;
	};

	// center of pointcloud
	//volatile float u = BoundingBox.GetCenter().X;
	//volatile float v = BoundingBox.GetCenter().Y;
	FMatrix ProjectionMatrix = ViewInfo.CalculateProjectionMatrix().Inverse();
	float u = RenderTexture->SizeX / 2.0;
	float v = RenderTexture->SizeY / 2.0;
	FVector MiddlePoint, MiddleDirection;
	FIntRect ScreenSize(0, 0, (int)RenderTexture->SizeX, (int)RenderTexture->SizeY);
	FSceneView::DeprojectScreenToWorld(FVector2D(u, v), ScreenSize, ProjectionMatrix, MiddlePoint, MiddleDirection);

	for (int y = BoundingBox.Min.Y; y <= BoundingBox.Max.Y; y++) {
		for (int x = BoundingBox.Min.X; x <= BoundingBox.Max.X; x++) {

			int i = toIndex(x, y);


			FVector PointLocation, PointDirection;
			FSceneView::DeprojectScreenToWorld(FVector2D(x, y), ScreenSize, ProjectionMatrix, PointLocation, PointDirection);

			// calculations in cm
			float z = FormatedImageData[i].A;
			// -> / 100 = in m -> * z = in 3D room
			volatile float x3d = (PointLocation.X - MiddlePoint.X) * (z / PointLocation.Z);
			volatile float y3d = (PointLocation.Y - MiddlePoint.Y) * (z / PointLocation.Z);

			FVector p;
			p.X = z ;
			p.Y = x3d ;
			p.Z = y3d ;

			PointCloud.Add(p);
		}
	}
	double end = FPlatformTime::Seconds();
	UE_LOG(LogTemp, Log, TEXT("Generated %i 3D points in %f milliseconds"), PointCloud.Num(), (end - start)*1000.0f);
}

bool UTryThingsOut::ComparePointsWithMesh(AActor* MeshInWorld, TArray<FVector> Points, FTransform RelativePointTransform, FString & CompareMessage)
{
	bool AllHit = true;

	FVector Min, Max;
	MeshInWorld->GetActorBounds(false, Min, Max);
	FBox MeshBounds(Min-Max, Min+Max);
	CompareMessage = "Mesh Bounds Origin: " + MeshBounds.GetCenter().ToString() +  ", " + MeshBounds.GetExtent().ToString() + " \n";
	CompareMessage += "Camera Transform: " + RelativePointTransform.ToString() + " \n";
	MeshBounds = MeshBounds.TransformBy(RelativePointTransform.Inverse());
	CompareMessage += "Relative Mesh Bounds Origin: " + MeshBounds.GetCenter().ToString() + ", " + MeshBounds.GetExtent().ToString() + " \n";
	
	for (const FVector& Point : Points)
	{
		FVector PointInWorld = Point; // RelativePointTransform.TransformVector(Point);
		CompareMessage += "Point " + PointInWorld.ToString();
		if (MeshBounds.IsInsideOrOn(PointInWorld)) {
			CompareMessage += " hits Bounds\n";
		}
		else {
			CompareMessage += " does NOT hit the Bounds!\n";
			AllHit = false;
		}
	}

	return AllHit;
}
