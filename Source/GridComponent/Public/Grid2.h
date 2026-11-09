#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Grid2.generated.h"

UENUM(BlueprintType)
enum class EGridType : uint8
{
    Orthogonal,    // 直角棋盘（原来的）
    Isometric      // 等距菱形 Isometric 2:1
};

UENUM(BlueprintType)
enum class EGridOrigin2 : uint8
{
    BottomLeft,
    BottomRight,
    TopLeft,
    TopRight,
    Center
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GRIDCOMPONENT_API UGrid2 : public USceneComponent
{
    GENERATED_BODY()
public:
    UGrid2();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
    EGridType GridType = EGridType::Isometric;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
    EGridOrigin2 GridOrigin = EGridOrigin2::BottomLeft;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
    FIntPoint GridSize = FIntPoint(10,10);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
    FVector2D CellSize = FVector2D(128.f,64.f);

    // Debug
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
    uint8 bIsDepthPriority = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
    bool bDrawDebugGrid = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
    bool bDrawDebugCellText = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
    float DebugLineZOffset = 20.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
    
    FColor DebugLineColor = FColor::Green;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
    FColor DebugTextColor = FColor::White;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
    float DebugLineThickness = 2.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
    float DebugTextScale = 1.f;

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    UFUNCTION(BlueprintPure, Category="Grid")
    FVector GetGridOriginOffset() const;

    UFUNCTION(BlueprintPure, Category="Grid")
    FVector GetGridCenterWorld() const;

    UFUNCTION(BlueprintPure, Category="Grid")
    FVector CellToWorld(const FIntPoint& Cell) const;

    UFUNCTION(BlueprintPure, Category="Grid")
    FIntPoint WorldToCell(const FVector& WorldPos) const;

    UFUNCTION(BlueprintPure, Category="Grid")
    FIntPoint Mirror(FIntPoint Cell) const;

    UFUNCTION(BlueprintPure, Category="Grid")
    bool IsCellValid(const FIntPoint& Cell) const;

    UFUNCTION(BlueprintPure, Category="Grid")
    bool IsWorldPosInsideGrid(const FVector& WorldPos) const;
    
 

#if !UE_BUILD_SHIPPING
    void DrawDebugGrid() const;
    void DrawDebugCellText() const;
#endif
};
