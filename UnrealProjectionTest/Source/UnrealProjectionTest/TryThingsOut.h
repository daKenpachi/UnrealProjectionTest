// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Camera/CameraTypes.h"
#include "IntRect.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Runtime/Engine/Classes/Components/SceneCaptureComponent2D.h"
#include "Runtime/Core/Public/Math/Box.h"

#include "TryThingsOut.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPROJECTIONTEST_API UTryThingsOut : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

		UFUNCTION(BlueprintPure, Category = "Try")
		static FBox2D ConvertBoundBox2D(FMinimalViewInfo ViewInfo, FBox2D Resolution, FVector Origin,  FVector Extend, TArray<FVector2D>& Pixels, TArray<FVector>& Points);

	UFUNCTION(BluePrintCallable, Category = "Try")
		static bool calcBoundingFromBox(USceneCaptureComponent2D* RenderComponent, FBox BoundingBox3D, FBox2D& BoxOut, TArray<FVector>& Points, TArray<FVector2D>& Points2D);

	UFUNCTION(BlueprintCallable, Category = "Try")
		static FBox2D calcBoundingFromBinary(UTextureRenderTarget2D * RenderTexture);

	UFUNCTION(BlueprintCallable, Category = "Try")
		static FBox2D calcBoundingFromColor(UTextureRenderTarget2D * RenderTexture, FLinearColor Color);

	UFUNCTION(BlueprintCallable, Category = "Try")
		static void RenderTarget2PointCloudWithView(USceneCaptureComponent2D* CaptureComponent, FBox2D BoundingBox, TArray<FVector>& PointCloud);

	UFUNCTION(BlueprintCallable, Category = "Try")
		static bool ComparePointsWithMesh(AActor* MeshInWorld, TArray<FVector> Points, FTransform RelativePointTransform, FString& CompareMessage);
};
