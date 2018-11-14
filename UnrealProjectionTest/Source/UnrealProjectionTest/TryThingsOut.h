// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Camera/CameraTypes.h"
#include "IntRect.h"

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

	UFUNCTION(BlueprintPure, Category = "Try")
		FBox2D calcBoundingFromBinary(UTextureRenderTarget2D * RenderTexture)
};
