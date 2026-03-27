// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/TextRenderComponent.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Grid.generated.h"

UENUM(BlueprintType)
enum class EGridOrigin : uint8
{
	BottomLeft     UMETA(DisplayName = "Bottom Left"),
	BottomRight    UMETA(DisplayName = "Bottom Right"),
	TopLeft        UMETA(DisplayName = "Top Left"),
	TopRight       UMETA(DisplayName = "Top Right"),
	Center         UMETA(DisplayName = "Center"),
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GRIDCOMPONENT_API UGrid : public UActorComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for this component's properties
	UGrid();
	FVector GetGridOriginOffset() const;
	
private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid",meta=(AllowPrivateAccess="true"))
	EGridOrigin GridOrigin = EGridOrigin::BottomLeft;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Grid",meta=(AllowPrivateAccess="true"))
	FVector2D CellSize=FIntPoint(100.f,100.f);
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Grid",meta=(AllowPrivateAccess="true"))
	FIntPoint GridSize=FIntPoint(20,20);
	
	UFUNCTION(BlueprintPure, Category="Grid",meta=(AllowPrivateAccess="true"))
	FIntPoint Mirror(FIntPoint Cell) const;


	UFUNCTION(BlueprintCallable, Category="Grid",meta=(AllowPrivateAccess="true"))
	FVector CellToWorld(const FIntPoint& Cell) const;
	
	UFUNCTION(BlueprintCallable, Category="Grid",meta=(AllowPrivateAccess="true"))
	FIntPoint WorldToCell(const FVector& World) const;
	
	UFUNCTION(BlueprintCallable, Category="Grid")
	bool IsCellValid(const FIntPoint& Cell) const;
	
	UFUNCTION(BlueprintCallable, Category="Grid")
	bool IsWorldPosInsideGrid(const FVector& WorldPos) const;
	
	// 是否绘制调试网格
	UPROPERTY(EditAnywhere, Category="Grid|Debug")
	bool bDrawDebugGrid = true;
	
	UPROPERTY(EditAnywhere, Category="Grid|Debug")
	bool bDrawDebugCellText = true;
	
	// 调试线颜色
	UPROPERTY(EditAnywhere, Category="Grid|Debug")
	FColor DebugLineColor = FColor::Green;
	
	UPROPERTY(EditAnywhere, Category="Grid|Debug")
	FColor DebugTextColor = FColor::Yellow;

	// 调试线粗细
	UPROPERTY(EditAnywhere, Category="Grid|Debug")
	float DebugLineThickness = 1.5f;
	
	UPROPERTY(EditAnywhere, Category="Grid|Debug")
	float DebugLineZOffset = 1.f;
	
	

	UPROPERTY(EditAnywhere, Category="Grid|Debug")
	float DebugTextScale = 1.0f;
	
	#if !UE_BUILD_SHIPPING
	void DrawDebugGrid() const;
	void DrawDebugCellText() const;
	#endif
	
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(Transient)
	TMap<FIntPoint, UTextRenderComponent*> CellTextMap;
};
